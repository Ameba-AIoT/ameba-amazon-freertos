/*
 * AWS IoT Device SDK for Embedded C 202412.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file mqtt_operations.c
 *
 * @brief This file provides wrapper functions for MQTT operations on a mutually
 * authenticated TLS connection.
 *
 * A mutually authenticated TLS connection is used to connect to the AWS IoT
 * MQTT message broker in this example. Define ROOT_CA_CERT_PATH,
 * CLIENT_CERT_PATH, and CLIENT_PRIVATE_KEY_PATH in demo_config.h to achieve
 * mutual authentication.
 */

/* Standard includes. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* POSIX includes. */
#include <unistd.h>

/* Config include. */
//#include "demo_config.h"
#include "fleet_provisioning_config.h"

/* Interface include. */
#include "mqtt_operations.h"

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/*Include backoff algorithm header for retry logic.*/
#include "backoff_algorithm.h"

/* AWS IoT Core TLS ALPN definitions for MQTT authentication */
#include "aws_iot_alpn_defs.h"

/**
 * Provide default values for undefined configuration settings.
 */
#ifndef AWS_MQTT_PORT
    #define AWS_MQTT_PORT    clientcredentialMQTT_BROKER_PORT
#endif

#ifndef NETWORK_BUFFER_SIZE
    #define NETWORK_BUFFER_SIZE    democonfigNETWORK_BUFFER_SIZE
#endif

/**
 * @brief Length of the AWS IoT endpoint.
 */
#define AWS_IOT_ENDPOINT_LENGTH                  ( ( uint16_t ) ( sizeof( AWS_IOT_ENDPOINT ) - 1 ) )

/**
 * @brief Length of the client identifier.
 */
#define CLIENT_IDENTIFIER_LENGTH                 ( ( uint16_t ) ( sizeof( CLIENT_IDENTIFIER ) - 1 ) )

/**
 * @brief The maximum number of retries for connecting to server.
 */
#define CONNECTION_RETRY_MAX_ATTEMPTS            ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying connection to server.
 */
#define CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS    ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for connection retry attempts.
 */
#define CONNECTION_RETRY_BACKOFF_BASE_MS         ( 500U )

/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define CONNACK_RECV_TIMEOUT_MS                  ( 1000U )

/**
 * @brief Maximum number of outgoing publishes maintained in the application
 * until an ack is received from the broker.
 */
#define MAX_OUTGOING_PUBLISHES                   ( 5U )

/**
 * @brief Invalid packet identifier for the MQTT packets. Zero is always an
 * invalid packet identifier as per MQTT 3.1.1 spec.
 */
#define MQTT_PACKET_ID_INVALID                   ( ( uint16_t ) 0U )

/**
 * @brief Timeout for MQTT_ProcessLoop function in milliseconds.
 */
#define MQTT_PROCESS_LOOP_TIMEOUT_MS             ( 5000U )

/**
 * @brief The maximum time interval in seconds which is allowed to elapse
 *  between two Control Packets.
 *
 *  It is the responsibility of the client to ensure that the interval between
 *  control packets being sent does not exceed the this keep-alive value. In the
 *  absence of sending any other control packets, the client MUST send a
 *  PINGREQ packet.
 */
#define MQTT_KEEP_ALIVE_INTERVAL_SECONDS         ( 60U )

/**
 * @brief Timeout in milliseconds for transport send and receive.
 */
#define TRANSPORT_SEND_RECV_TIMEOUT_MS           ( 1000U )
#define mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS TRANSPORT_SEND_RECV_TIMEOUT_MS
/**
 * @brief The MQTT metrics string expected by AWS IoT MQTT Broker.
 */
#define METRICS_STRING                           "?SDK=" OS_NAME "&Version=" OS_VERSION "&Platform=" HARDWARE_PLATFORM_NAME "&MQTTLib=" MQTT_LIB

/**
 * @brief The length of the MQTT metrics string.
 */
#define METRICS_STRING_LENGTH                    ( ( uint16_t ) ( sizeof( METRICS_STRING ) - 1 ) )

/**
 * @brief The length of the outgoing publish records array used by the coreMQTT
 * library to track QoS > 0 packet ACKS for outgoing publishes.
 */
#define OUTGOING_PUBLISH_RECORD_LEN              ( 10U )

/**
 * @brief The length of the incoming publish records array used by the coreMQTT
 * library to track QoS > 0 packet ACKS for incoming publishes.
 */
#define INCOMING_PUBLISH_RECORD_LEN              ( 10U )
/*-----------------------------------------------------------*/

/**
 * @brief Structure to keep the MQTT publish packets until an ack is received
 * for QoS1 publishes.
 */
typedef struct PublishPackets
{
    /**
     * @brief Packet identifier of the publish packet.
     */
    uint16_t packetId;

    /**
     * @brief Publish info of the publish packet.
     */
    MQTTPublishInfo_t pubInfo;
} PublishPackets_t;

/* Each compilation unit must define the NetworkContext struct. */
struct NetworkContext
{
    SecureSocketsTransportParams_t * pParams;
};
/*-----------------------------------------------------------*/

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/**
 * @brief Packet Identifier updated when an ACK packet is received.
 *
 * It is used to match an expected ACK for a transmitted packet.
 */
static uint16_t globalAckPacketIdentifier = 0U;

/**
 * @brief Packet Identifier generated when Subscribe request was sent to the broker.
 *
 * It is used to match received Subscribe ACK to the transmitted subscribe
 * request.
 */
static uint16_t globalSubscribePacketIdentifier = 0U;

/**
 * @brief Packet Identifier generated when Unsubscribe request was sent to the broker.
 *
 * It is used to match received Unsubscribe ACK to the transmitted unsubscribe
 * request.
 */
static uint16_t globalUnsubscribePacketIdentifier = 0U;

/**
 * @brief Array to keep the outgoing publish messages.
 *
 * These stored outgoing publish messages are kept until a successful ack
 * is received.
 */
static PublishPackets_t outgoingPublishPackets[ MAX_OUTGOING_PUBLISHES ] = { 0 };

/**
 * @brief The network buffer must remain valid for the lifetime of the MQTT context.
 */
static uint8_t buffer[ NETWORK_BUFFER_SIZE ];

/**
 * @brief The MQTT context used for MQTT operation.
 */
static MQTTContext_t mqttContext = { 0 };

/**
 * @brief The network context used for MbedTLS operation.
 */
static NetworkContext_t networkContext = { 0 };

/**
 * @brief The parameters for MbedTLS operation.
 */
//static MbedtlsPkcs11Context_t tlsContext = { 0 };
static SecureSocketsTransportParams_t tlsTransportParams = { 0 };

/**
 * @brief The flag to indicate that the mqtt session is established.
 */
static bool mqttSessionEstablished = false;

/**
 * @brief Callback registered when calling EstablishMqttSession to get incoming
 * publish messages.
 */
static MQTTPublishCallback_t appPublishCallback = NULL;

/**
* @brief Optional client identifier override set by Fleet_SetClientIdentifier.
* NULL means fall back to democonfigCLIENT_IDENTIFIER.
*/
static const char * pOverrideClientIdentifier = NULL;
static uint16_t overrideClientIdentifierLength = 0U;

void Fleet_SetClientIdentifier( const char * pClientId,
                            uint16_t length )
{
    pOverrideClientIdentifier = pClientId;
    overrideClientIdentifierLength = length;
}

/**
 * @brief Array to track the outgoing publish records for outgoing publishes
 * with QoS > 0.
 *
 * This is passed into #MQTT_InitStatefulQoS to allow for QoS > 0.
 *
 */
static MQTTPubAckInfo_t pOutgoingPublishRecords[ OUTGOING_PUBLISH_RECORD_LEN ];

/**
 * @brief Array to track the incoming publish records for incoming publishes
 * with QoS > 0.
 *
 * This is passed into #MQTT_InitStatefulQoS to allow for QoS > 0.
 *
 */
static MQTTPubAckInfo_t pIncomingPublishRecords[ INCOMING_PUBLISH_RECORD_LEN ];
/*-----------------------------------------------------------*/

/**
 * @brief The random number generator to use for exponential backoff with
 * jitter retry logic.
 *
 * @return The generated random number.
 */
static uint32_t generateRandomNumber( void );

/**
 * @brief Connect to the MQTT broker with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout. Timeout value
 * exponentially increases until maximum timeout value is reached or the number
 * of attempts are exhausted.
 *
 * @param[out] pNetworkContext The created network context.
 * @param[in] p11Session The PKCS #11 session to use.
 * @param[in] pClientCertLabel The client certificate PKCS #11 label to use.
 * @param[in] pPrivateKeyLabel The private key PKCS #11 label for the client certificate.
 *
 * @return false on failure; true on successful connection.
 */
static bool connectToBrokerWithBackoffRetries( NetworkContext_t * pNetworkContext,
                                               CK_SESSION_HANDLE p11Session,
                                               char * pClientCertLabel,
                                               char * pPrivateKeyLabel );

/**
 * @brief Get the free index in the #outgoingPublishPackets array at which an
 * outgoing publish can be stored.
 *
 * @param[out] pIndex The index at which an outgoing publish can be stored.
 *
 * @return false if no more publishes can be stored;
 * true if an index to store the next outgoing publish is obtained.
 */
static bool getNextFreeIndexForOutgoingPublishes( uint8_t * pIndex );

/**
 * @brief Clean up the outgoing publish at given index from the
 * #outgoingPublishPackets array.
 *
 * @param[in] index The index at which a publish message has to be cleaned up.
 */
static void cleanupOutgoingPublishAt( uint8_t index );

/**
 * @brief Clean up all the outgoing publishes in the #outgoingPublishPackets array.
 */
static void cleanupOutgoingPublishes( void );

/**
 * @brief Clean up the publish packet with the given packet id in the
 * #outgoingPublishPackets array.
 *
 * @param[in] packetId Packet id of the packet to be clean.
 */
static void cleanupOutgoingPublishWithPacketID( uint16_t packetId );

/**
 * @brief Callback registered with the MQTT library.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pDeserializedInfo Deserialized information from the incoming packet.
 */
static void mqttCallback( MQTTContext_t * pMqttContext,
                          MQTTPacketInfo_t * pPacketInfo,
                          MQTTDeserializedInfo_t * pDeserializedInfo );

/**
 * @brief Resend the publishes if a session is re-established with the broker.
 *
 * This function handles the resending of the QoS1 publish packets, which are
 * maintained locally.
 *
 * @param[in] pMqttContext The MQTT context pointer.
 *
 * @return true if all the unacknowledged QoS1 publishes are re-sent successfully;
 * false otherwise.
 */
static bool handlePublishResend( MQTTContext_t * pMqttContext );

/**
 * @brief Wait for an expected ACK packet to be received.
 *
 * This function handles waiting for an expected ACK packet by calling
 * #MQTT_ProcessLoop and waiting for #mqttCallback to set the global ACK
 * packet identifier to the expected ACK packet identifier.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] usPacketIdentifier Packet identifier for expected ACK packet.
 * @param[in] ulTimeout Maximum duration to wait for expected ACK packet.
 *
 * @return true if the expected ACK packet was received, false otherwise.
 */
static bool waitForPacketAck( MQTTContext_t * pMqttContext,
                              uint16_t usPacketIdentifier,
                              uint32_t ulTimeout );

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs( void );
/*-----------------------------------------------------------*/

static uint32_t prvGetTimeMs( void )
{
    TickType_t xTickCount = 0;
    uint32_t ulTimeMs = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = ( uint32_t ) xTickCount * MILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = ( uint32_t ) ( ulTimeMs - ulGlobalEntryTimeMs );

    return ulTimeMs;
}


static uint32_t generateRandomNumber()
{
    return( ( uint32_t ) rand() );
}

static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams )
{
    BaseType_t xReturnStatus = pdFAIL;
    uint16_t usNextRetryBackOff = 0U;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;

    /**
     * To calculate the backoff period for the next retry attempt, we will
     * generate a random number to provide to the backoffAlgorithm library.
     *
     * Note: The PKCS11 module is used to generate the random number as it allows access
     * to a True Random Number Generator (TRNG) if the vendor platform supports it.
     * It is recommended to use a random number generator seeded with a device-specific
     * entropy source so that probability of collisions from devices in connection retries
     * is mitigated.
     */
    uint32_t ulRandomNum = 0;

    if( xPkcs11GenerateRandomNumber( ( uint8_t * ) &ulRandomNum,
                                     sizeof( ulRandomNum ) ) == pdPASS )
    {
        /* Get back-off value (in milliseconds) for the next retry attempt. */
        xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( pxRetryParams, ulRandomNum, &usNextRetryBackOff );

        if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
        {
            LogError( ( "All retry attempts have exhausted. Operation will not be retried" ) );
        }
        else if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
        {
            /* Perform the backoff delay. */
            vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );

            xReturnStatus = pdPASS;

            LogInfo( ( "Retry attempt %lu out of maximum retry attempts %lu.",
                       ( pxRetryParams->attemptsDone + 1 ),
                       pxRetryParams->maxRetryAttempts ) );
        }
    }
    else
    {
        LogError( ( "Unable to retry operation with broker: Random number generation failed" ) );
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static bool connectToBrokerWithBackoffRetries( NetworkContext_t * pxNetworkContext,
                                               CK_SESSION_HANDLE p11Session,
                                               char * pClientCertLabel,
                                               char * pPrivateKeyLabel )
{
    BaseType_t xBackoffStatus = pdFALSE;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    BackoffAlgorithmContext_t xReconnectParams;;
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketsConfig = { 0 };
    uint16_t nextRetryBackOff = 0U;

    /* Set the pParams member of the network context with desired transport. */
    // pNetworkContext->pParams = &tlsContext;
    pxNetworkContext->pParams = &tlsTransportParams;

    /* Initialize credentials for establishing TLS session. */
    // tlsCredentials.pRootCaPath = ROOT_CA_CERT_PATH;
    // tlsCredentials.pClientCertLabel = pClientCertLabel;
    // tlsCredentials.pPrivateKeyLabel = pPrivateKeyLabel;
    // tlsCredentials.p11Session = p11Session;

    /* Set the credentials for establishing a TLS connection. */
    /* Initialize server information. */
    xServerInfo.pHostName = democonfigMQTT_BROKER_ENDPOINT;
    xServerInfo.hostNameLength = strlen( democonfigMQTT_BROKER_ENDPOINT );
    xServerInfo.port = democonfigMQTT_BROKER_PORT;

    /* AWS IoT requires devices to send the Server Name Indication (SNI)
     * extension to the Transport Layer Security (TLS) protocol and provide
     * the complete endpoint address in the host_name field. Details about
     * SNI for AWS IoT can be found in the link below.
     * https://docs.aws.amazon.com/iot/latest/developerguide/transport-security.html
     */
    // tlsCredentials.disableSni = false;

    /* Configure credentials for TLS mutual authenticated session. */
    xSocketsConfig.enableTls = true;
    xSocketsConfig.pAlpnProtos = NULL;
    xSocketsConfig.maxFragmentLength = 0;
    xSocketsConfig.disableSni = false;
    xSocketsConfig.pRootCa = democonfigROOT_CA_PEM;
    xSocketsConfig.rootCaSize = sizeof( democonfigROOT_CA_PEM );
    xSocketsConfig.sendTimeoutMs = mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketsConfig.recvTimeoutMs = mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketsConfig.p11Session = p11Session;
    xSocketsConfig.pClientCertLabel = pClientCertLabel;
    xSocketsConfig.pPrivateKeyLabel = pPrivateKeyLabel;

    if( AWS_MQTT_PORT == 443 )
    {
        static const char * alpnProtoArray[] = AWS_IOT_ALPN_MQTT_CA_AUTH_MBEDTLS;

        /* Pass the ALPN protocol name depending on the port being used.
         * Please see more details about the ALPN protocol for AWS IoT MQTT endpoint
         * in the link below.
         * https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/
         */
        // tlsCredentials.pAlpnProtos = alpnProtoArray;
        xSocketsConfig.pAlpnProtos = alpnProtoArray;
    }

    /* Initialize reconnect attempts and interval */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       CONNECTION_RETRY_BACKOFF_BASE_MS,
                                       CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS,
                                       CONNECTION_RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase till maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TLS session with the MQTT broker. This example connects to
         * the MQTT broker as specified in democonfigMQTT_BROKER_ENDPOINT and
         * democonfigMQTT_BROKER_PORT at the top of this file. */
        LogInfo( ( "Creating a TLS connection to %s:%u.",
                   democonfigMQTT_BROKER_ENDPOINT,
                   democonfigMQTT_BROKER_PORT ) );
        /* Attempt to create a mutually authenticated TLS connection. */
        xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext,
                                                         &xServerInfo,
                                                         &xSocketsConfig );

        if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            LogWarn( ( "Connection to the broker failed. Attempting connection retry after backoff delay." ) );

            /* As the connection attempt failed, we will retry the connection after an
             * exponential backoff with jitter delay. */

            /* Calculate the backoff period for the next retry attempt and perform the wait operation. */
            xBackoffStatus = prvBackoffForRetry( &xReconnectParams );
        }
    } while( ( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS ) && ( xBackoffStatus == pdPASS ) );

    // return ( xNetworkStatus == TRANSPORT_SOCKET_STATUS_SUCCESS ) ? pdPASS : pdFAIL;
    return ( xNetworkStatus == TRANSPORT_SOCKET_STATUS_SUCCESS );
}
/*-----------------------------------------------------------*/

static bool getNextFreeIndexForOutgoingPublishes( uint8_t * pIndex )
{
    bool returnStatus = false;
    uint8_t index = 0;

    assert( outgoingPublishPackets != NULL );
    assert( pIndex != NULL );

    for( index = 0; index < MAX_OUTGOING_PUBLISHES; index++ )
    {
        /* A free index is marked by invalid packet id. Check if the the index
         * has a free slot. */
        if( outgoingPublishPackets[ index ].packetId == MQTT_PACKET_ID_INVALID )
        {
            returnStatus = true;
            break;
        }
    }

    /* Copy the available index into the output param. */
    if( returnStatus == true )
    {
        *pIndex = index;
    }

    return returnStatus;
}
/*-----------------------------------------------------------*/

static void cleanupOutgoingPublishAt( uint8_t index )
{
    assert( outgoingPublishPackets != NULL );
    assert( index < MAX_OUTGOING_PUBLISHES );

    /* Clear the outgoing publish packet. */
    ( void ) memset( &( outgoingPublishPackets[ index ] ),
                     0x00,
                     sizeof( outgoingPublishPackets[ index ] ) );
}
/*-----------------------------------------------------------*/

static void cleanupOutgoingPublishes( void )
{
    assert( outgoingPublishPackets != NULL );

    /* Clean up all the outgoing publish packets. */
    ( void ) memset( outgoingPublishPackets, 0x00, sizeof( outgoingPublishPackets ) );
}
/*-----------------------------------------------------------*/

static void cleanupOutgoingPublishWithPacketID( uint16_t packetId )
{
    uint8_t index = 0;

    assert( outgoingPublishPackets != NULL );
    assert( packetId != MQTT_PACKET_ID_INVALID );

    /* Clean up the saved outgoing publish with packet Id equal to packetId. */
    for( index = 0; index < MAX_OUTGOING_PUBLISHES; index++ )
    {
        if( outgoingPublishPackets[ index ].packetId == packetId )
        {
            cleanupOutgoingPublishAt( index );

            LogDebug( ( "Cleaned up outgoing publish packet with packet id %u.",
                        packetId ) );

            break;
        }
    }
}
/*-----------------------------------------------------------*/

static void mqttCallback( MQTTContext_t * pMqttContext,
                          MQTTPacketInfo_t * pPacketInfo,
                          MQTTDeserializedInfo_t * pDeserializedInfo )
{
    uint16_t packetIdentifier;

    assert( pMqttContext != NULL );
    assert( pPacketInfo != NULL );
    assert( pDeserializedInfo != NULL );

    /* Suppress the unused parameter warning when asserts are disabled in
     * build. */
    ( void ) pMqttContext;

    packetIdentifier = pDeserializedInfo->packetIdentifier;

    /* Handle an incoming publish. The lower 4 bits of the publish packet
     * type is used for the dup, QoS, and retain flags. Hence masking
     * out the lower bits to check if the packet is publish. */
    if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        assert( pDeserializedInfo->pPublishInfo != NULL );

        /* Invoke the application callback for incoming publishes. */
        if( appPublishCallback != NULL )
        {
            appPublishCallback( pDeserializedInfo->pPublishInfo, packetIdentifier );
        }
    }
    else
    {
        /* Handle other packets. */
        switch( pPacketInfo->type )
        {
            case MQTT_PACKET_TYPE_SUBACK:
                LogDebug( ( "MQTT Packet type SUBACK received." ) );

                /* Make sure the ACK packet identifier matches with the request
                 * packet identifier. */
                assert( globalSubscribePacketIdentifier == packetIdentifier );

                /* Update the global ACK packet identifier. */
                globalAckPacketIdentifier = packetIdentifier;
                break;

            case MQTT_PACKET_TYPE_UNSUBACK:
                LogDebug( ( "MQTT Packet type UNSUBACK received." ) );

                /* Make sure the ACK packet identifier matches with the request
                 * packet identifier. */
                assert( globalUnsubscribePacketIdentifier == packetIdentifier );

                /* Update the global ACK packet identifier. */
                globalAckPacketIdentifier = packetIdentifier;
                break;

            case MQTT_PACKET_TYPE_PINGRESP:

                /* We do not expect to receive PINGRESP as we are using
                 * MQTT_ProcessLoop. */
                LogWarn( ( "PINGRESP should not be received by the application "
                           "callback when using MQTT_ProcessLoop." ) );
                break;

            case MQTT_PACKET_TYPE_PUBACK:
                LogDebug( ( "PUBACK received for packet id %u.",
                            packetIdentifier ) );

                /* Update the global ACK packet identifier. */
                globalAckPacketIdentifier = packetIdentifier;

                /* Cleanup the publish packet from the #outgoingPublishPackets
                 * array when a PUBACK is received. */
                cleanupOutgoingPublishWithPacketID( packetIdentifier );
                break;

            /* Any other packet type is invalid. */
            default:
                LogError( ( "Unknown packet type received:(%02x).",
                            pPacketInfo->type ) );
        }
    }
}
/*-----------------------------------------------------------*/

static bool handlePublishResend( MQTTContext_t * pMqttContext )
{
    bool returnStatus = false;
    MQTTStatus_t mqttStatus = MQTTSuccess;
    uint8_t index = 0U;

    assert( outgoingPublishPackets != NULL );

    /* MQTTv5: Create property builder to handle MQTTv5 properties */
    MQTTPropBuilder_t propertyBuilder ;
    uint8_t buf[100] ;
    size_t bufLength = sizeof(buf);
    MQTTPropertyBuilder_Init(&propertyBuilder, buf, bufLength) ;

    /* MQTTv5: Add publish properties */                        
    MQTTPropAdd_PayloadFormat(&propertyBuilder, 1, &(uint8_t){ MQTT_PACKET_TYPE_PUBLISH } );
    MQTTPropAdd_TopicAlias(&propertyBuilder, 1, &(uint8_t){ MQTT_PACKET_TYPE_PUBLISH } );


    /* Resend all the QoS1 publishes still in the #outgoingPublishPackets array.
     * These are the publishes that haven't received a PUBACK yet. When a PUBACK
     * is received, the corresponding publish is removed from the array. */
    for( index = 0U; index < MAX_OUTGOING_PUBLISHES; index++ )
    {
        if( outgoingPublishPackets[ index ].packetId != MQTT_PACKET_ID_INVALID )
        {
            outgoingPublishPackets[ index ].pubInfo.dup = true;

            LogDebug( ( "Sending duplicate PUBLISH with packet id %u.",
                        outgoingPublishPackets[ index ].packetId ) );
            mqttStatus = MQTT_Publish( pMqttContext,
                                       &outgoingPublishPackets[ index ].pubInfo,
                                       outgoingPublishPackets[ index ].packetId,
                                       &propertyBuilder );

            if( mqttStatus != MQTTSuccess )
            {
                LogError( ( "Sending duplicate PUBLISH for packet id %u "
                            " failed with status %s.",
                            outgoingPublishPackets[ index ].packetId,
                            MQTT_Status_strerror( mqttStatus ) ) );
                break;
            }
            else
            {
                LogDebug( ( "Sent duplicate PUBLISH successfully for packet id %u.",
                            outgoingPublishPackets[ index ].packetId ) );
            }
        }
    }

    /* Were all the unacknowledged QoS1 publishes successfully re-sent? */
    if( index == MAX_OUTGOING_PUBLISHES )
    {
        returnStatus = true;
    }

    return returnStatus;
}
/*-----------------------------------------------------------*/

static bool waitForPacketAck( MQTTContext_t * pMqttContext,
                              uint16_t usPacketIdentifier,
                              uint32_t ulTimeout )
{
    uint32_t ulMqttProcessLoopEntryTime;
    uint32_t ulMqttProcessLoopTimeoutTime;
    uint32_t ulCurrentTime;

    MQTTStatus_t eMqttStatus = MQTTSuccess;
    bool xStatus = false;

    /* Reset the ACK packet identifier being received. */
    globalAckPacketIdentifier = 0U;

    ulCurrentTime = pMqttContext->getTime();
    ulMqttProcessLoopEntryTime = ulCurrentTime;
    ulMqttProcessLoopTimeoutTime = ulCurrentTime + ulTimeout;

    /* Call MQTT_ProcessLoop multiple times until the expected packet ACK
     * is received, a timeout happens, or MQTT_ProcessLoop fails. */
    while( ( globalAckPacketIdentifier != usPacketIdentifier ) &&
           ( ulCurrentTime < ulMqttProcessLoopTimeoutTime ) &&
           ( eMqttStatus == MQTTSuccess || eMqttStatus == MQTTNeedMoreBytes ) )
    {
        /* Event callback will set #globalAckPacketIdentifier when receiving
         * appropriate packet. */
        eMqttStatus = MQTT_ProcessLoop( pMqttContext );
        ulCurrentTime = pMqttContext->getTime();
    }

    if( ( ( eMqttStatus != MQTTSuccess ) && ( eMqttStatus != MQTTNeedMoreBytes ) ) ||
        ( globalAckPacketIdentifier != usPacketIdentifier ) )
    {
        LogError( ( "MQTT_ProcessLoop failed to receive ACK packet: Expected ACK Packet ID=%02X, LoopDuration=%u, Status=%s",
                    usPacketIdentifier,
                    ( ulCurrentTime - ulMqttProcessLoopEntryTime ),
                    MQTT_Status_strerror( eMqttStatus ) ) );
    }
    else
    {
        xStatus = true;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

bool Fleet_EstablishMqttSession( MQTTPublishCallback_t publishCallback,
                           CK_SESSION_HANDLE p11Session,
                           char * pClientCertLabel,
                           char * pPrivateKeyLabel )
{
    bool returnStatus = false;
    MQTTStatus_t mqttStatus;
    MQTTConnectInfo_t connectInfo;
    MQTTFixedBuffer_t networkBuffer;
    TransportInterface_t transport = { NULL };
    bool createCleanSession = false;
    MQTTContext_t * pMqttContext = &mqttContext;
    NetworkContext_t * pNetworkContext = &networkContext;
    bool sessionPresent = false;

    assert( pMqttContext != NULL );
    assert( pNetworkContext != NULL );

    /* Initialize the mqtt context and network context. */
    ( void ) memset( pMqttContext, 0U, sizeof( MQTTContext_t ) );
    ( void ) memset( pNetworkContext, 0U, sizeof( NetworkContext_t ) );

    returnStatus = connectToBrokerWithBackoffRetries( pNetworkContext,
                                                      p11Session,
                                                      pClientCertLabel,
                                                      pPrivateKeyLabel );

    if( returnStatus != true )
    {
        /* Log an error to indicate connection failure after all
         * reconnect attempts are over. */
        LogError( ( "Failed to connect to MQTT broker %.*s.",
                    sizeof(democonfigMQTT_BROKER_ENDPOINT),
                    democonfigMQTT_BROKER_ENDPOINT ) );
    }
    else
    {
        /* Fill in TransportInterface send and receive function pointers.
         * For this demo, TCP sockets are used to send and receive data
         * from the network. pNetworkContext is an SSL context for OpenSSL.*/
        transport.pNetworkContext = pNetworkContext;
        transport.send = SecureSocketsTransport_Send;
        transport.recv = SecureSocketsTransport_Recv;
        transport.writev = NULL;

        /* Fill the values for network buffer. */
        networkBuffer.pBuffer = buffer;
        networkBuffer.size = NETWORK_BUFFER_SIZE;

        /* Remember the publish callback supplied. */
        appPublishCallback = publishCallback;

        /* Initialize the MQTT library. */
        mqttStatus = MQTT_Init( pMqttContext,
                                &transport,
                                prvGetTimeMs,
                                mqttCallback,
                                &networkBuffer );

        if( mqttStatus != MQTTSuccess )
        {
            returnStatus = false;
            LogError( ( "MQTT_Init failed with status %s.",
                        MQTT_Status_strerror( mqttStatus ) ) );
        }
        else
        {
            /* MQTTv5: allocate buffer for publish acknowledgements */
            uint8_t propertyBuffer[500];
            mqttStatus = MQTT_InitStatefulQoS( pMqttContext,
                                               pOutgoingPublishRecords,
                                               OUTGOING_PUBLISH_RECORD_LEN,
                                               pIncomingPublishRecords,
                                               INCOMING_PUBLISH_RECORD_LEN,
                                               propertyBuffer,
                                               sizeof(propertyBuffer) );

            if( mqttStatus != MQTTSuccess )
            {
                returnStatus = false;
                LogError( ( "MQTT_InitStatefulQoS failed with status %s.",
                            MQTT_Status_strerror( mqttStatus ) ) );
            }
            else
            {
                /* Establish an MQTT session by sending a CONNECT packet. */

                /* If #createCleanSession is true, start with a clean session
                 * i.e. direct the MQTT broker to discard any previous session data.
                 * If #createCleanSession is false, direct the broker to attempt to
                 * reestablish a session which was already present. */
                connectInfo.cleanSession = createCleanSession;

                /* The client identifier is used to uniquely identify this MQTT client to
                 * the MQTT broker. In a production device the identifier can be something
                 * unique, such as a device serial number. */
                /* Detect if a previous identifier was already saved from a commissioning attempt, otherwise use default cred */
                if( pOverrideClientIdentifier != NULL )
                {
                    connectInfo.pClientIdentifier = pOverrideClientIdentifier;
                    connectInfo.clientIdentifierLength = overrideClientIdentifierLength;
                }
                else
                {
                    connectInfo.pClientIdentifier = democonfigCLIENT_IDENTIFIER;
                    connectInfo.clientIdentifierLength = ( uint16_t ) strlen( democonfigCLIENT_IDENTIFIER );
                }


                /* The maximum time interval in seconds which is allowed to elapse
                 * between two Control Packets.
                 * It is the responsibility of the client to ensure that the interval between
                 * control packets being sent does not exceed the this keep-alive value. In the
                 * absence of sending any other control packets, the client MUST send a
                 * PINGREQ packet. */
                connectInfo.keepAliveSeconds = MQTT_KEEP_ALIVE_INTERVAL_SECONDS;

                /* Username and password for authentication. Not used in this demo. */
                connectInfo.pUserName = AWS_IOT_METRICS_STRING;
                connectInfo.userNameLength = AWS_IOT_METRICS_STRING_LENGTH;
                connectInfo.pPassword = NULL;
                connectInfo.passwordLength = 0U;

                /* MQTTv5: Create property builder to handle MQTTv5 properties */
                MQTTPropBuilder_t connectionProperties ;
                uint8_t buf[500] ;
                size_t bufLength = sizeof(buf);
                MQTTPropertyBuilder_Init(&connectionProperties, buf, bufLength) ;

                /* MQTTv5: If using property builder, must set packet size */
                MQTTPropAdd_MaxPacketSize(&connectionProperties, NETWORK_BUFFER_SIZE, &(uint8_t){ MQTT_PACKET_TYPE_CONNECT } );
                MQTTPropAdd_RequestProbInfo(&connectionProperties, 1, NULL);

                /* MQTTv5: Set connection property (e.g session expiry) if needed */
                // uint32_t sessionExpiryInterval = 100 ; // 100ms
                // MQTTPropAdd_SessionExpiry(&connectionProperties, sessionExpiryInterval, &(uint8_t){ MQTT_PACKET_TYPE_CONNECT } );

                /* Send an MQTT CONNECT packet to the broker. */
                mqttStatus = MQTT_Connect( pMqttContext,
                                           &connectInfo,
                                           NULL,
                                           CONNACK_RECV_TIMEOUT_MS,
                                           &sessionPresent,
                                           &connectionProperties,
                                           NULL );

                if( mqttStatus != MQTTSuccess )
                {
                    returnStatus = false;
                    LogError( ( "Connection with MQTT broker failed with status %s.",
                                MQTT_Status_strerror( mqttStatus ) ) );
                }
                else
                {
                    LogDebug( ( "MQTT connection successfully established with broker." ) );
                }
            }
        }

        if( returnStatus == true )
        {
            /* Keep a flag for indicating if MQTT session is established. This
             * flag will mark that an MQTT DISCONNECT has to be sent at the end
             * of the demo even if there are intermediate failures. */
            mqttSessionEstablished = true;
        }

        if( returnStatus == true )
        {
            /* Check if a session is present and if there are any outgoing
             * publishes that need to be resent. Resending unacknowledged
             * publishes is needed only if the broker is re-establishing a
             * session that was already present. */
            if( sessionPresent == true )
            {
                LogDebug( ( "An MQTT session with broker is re-established. "
                            "Resending unacked publishes." ) );

                /* Handle all the resend of publish messages. */
                returnStatus = handlePublishResend( &mqttContext );
            }
            else
            {
                LogDebug( ( "A clean MQTT connection is established."
                            " Cleaning up all the stored outgoing publishes." ) );

                /* Clean up the outgoing publishes waiting for ack as this new
                 * connection doesn't re-establish an existing session. */
                cleanupOutgoingPublishes();
            }
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

bool Fleet_DisconnectMqttSession( void )
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    bool returnStatus = false;
    MQTTContext_t * pMqttContext = &mqttContext;
    NetworkContext_t * pNetworkContext = &networkContext;

    assert( pMqttContext != NULL );
    assert( pNetworkContext != NULL );

    if( mqttSessionEstablished == true )
    {
        /* MQTTv5: Create property builder to handle MQTTv5 properties */
        MQTTPropBuilder_t propertyBuilder = { 0 };
        uint8_t propertyBuffer[100];
        MQTTPropertyBuilder_Init(&propertyBuilder,
                                propertyBuffer,
                                sizeof(propertyBuffer));
        /* MQTTv5: Add disconnect properties */
        MQTTPropAdd_ReasonString(&propertyBuilder,
                                "Normal shutdown",
                                strlen("Normal shutdown"),
                                &(uint8_t){ MQTT_PACKET_TYPE_DISCONNECT });
        MQTTSuccessFailReasonCode_t reason = MQTT_REASON_DISCONNECT_NORMAL_DISCONNECTION;

        /* Send DISCONNECT. */
        mqttStatus = MQTT_Disconnect( pMqttContext,
                                       &propertyBuilder,
                                       &reason );

        if( mqttStatus != MQTTSuccess )
        {
            LogError( ( "Sending MQTT DISCONNECT failed with status=%u.",
                        mqttStatus ) );
        }
        else
        {
            /* MQTT DISCONNECT sent successfully. */
            returnStatus = true;
        }
    }

    /* End TLS session, then close TCP connection. */
    // ( void ) Mbedtls_Pkcs11_Disconnect( pNetworkContext );
    SecureSocketsTransport_Disconnect( pNetworkContext );

    return returnStatus;
}
/*-----------------------------------------------------------*/

bool Fleet_SubscribeToTopic( const char * pTopicFilter,
                       uint16_t topicFilterLength )
{
    bool returnStatus = false;
    MQTTStatus_t mqttStatus;
    MQTTContext_t * pMqttContext = &mqttContext;
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pMqttContext != NULL );
    assert( pTopicFilter != NULL );
    assert( topicFilterLength > 0 );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* This example subscribes to only one topic and uses QOS1. */
    pSubscriptionList[ 0 ].qos = MQTTQoS1;
    pSubscriptionList[ 0 ].pTopicFilter = pTopicFilter;
    pSubscriptionList[ 0 ].topicFilterLength = topicFilterLength;

    /* Generate packet identifier for the SUBSCRIBE packet. */
    globalSubscribePacketIdentifier = MQTT_GetPacketId( pMqttContext );

    /* MQTTv5: Create property builder to handle MQTTv5 properties */
    MQTTPropBuilder_t propertyBuilder;
    uint8_t buf[100];
    size_t bufLength = sizeof(buf);
    MQTTPropertyBuilder_Init(&propertyBuilder, buf, bufLength) ;

    /* MQTTv5: Add properties */
    /* WARNING: AWS does not support subscription ID */
    //MQTTPropAdd_SubscriptionId(&propertyBuilder, 1, &(uint8_t){ MQTT_PACKET_TYPE_SUBSCRIBE } );

    /* Send SUBSCRIBE packet. */
    mqttStatus = MQTT_Subscribe( pMqttContext,
                                 pSubscriptionList,
                                 sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                                 globalSubscribePacketIdentifier,
                                 &propertyBuilder );

    if( mqttStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send SUBSCRIBE packet to broker with error = %s.",
                    MQTT_Status_strerror( mqttStatus ) ) );
    }
    else
    {
        LogDebug( ( "SUBSCRIBE topic %.*s to broker.",
                    topicFilterLength,
                    pTopicFilter ) );

        /* Process incoming packet from the broker. Acknowledgment for subscription
         * ( SUBACK ) will be received here. However after sending the subscribe, the
         * client may receive a publish before it receives a subscribe ack. Since this
         * demo is subscribing to the topic to which no one is publishing, probability
         * of receiving publish message before subscribe ack is zero; but application
         * must be ready to receive any packet. This demo uses MQTT_ProcessLoop to
         * receive packet from network. */
        returnStatus = waitForPacketAck( pMqttContext,
                                         globalSubscribePacketIdentifier,
                                         MQTT_PROCESS_LOOP_TIMEOUT_MS );
    }

    return returnStatus;
}
/*-----------------------------------------------------------*/

bool Fleet_UnsubscribeFromTopic( const char * pTopicFilter,
                           uint16_t topicFilterLength )
{
    bool returnStatus = false;
    MQTTStatus_t mqttStatus;
    MQTTContext_t * pMqttContext = &mqttContext;
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pMqttContext != NULL );
    assert( pTopicFilter != NULL );
    assert( topicFilterLength > 0 );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* This example subscribes to only one topic and uses QOS1. */
    pSubscriptionList[ 0 ].qos = MQTTQoS1;
    pSubscriptionList[ 0 ].pTopicFilter = pTopicFilter;
    pSubscriptionList[ 0 ].topicFilterLength = topicFilterLength;

    /* Generate packet identifier for the UNSUBSCRIBE packet. */
    globalUnsubscribePacketIdentifier = MQTT_GetPacketId( pMqttContext );

    /* MQTTv5: Create property builder to handle MQTTv5 properties */
    MQTTPropBuilder_t propertyBuilder ;
    uint8_t buf[100] ;
    size_t bufLength = sizeof(buf);
    MQTTPropertyBuilder_Init(&propertyBuilder, buf, bufLength) ;

    /* MQTTv5: Create sample user custom property */
    MQTTUserProperty_t userProperty = {
        .pKey = "key",
        .keyLength = strlen("key"),
        .pValue = "value",
        .valueLength = strlen("value")
    };
    MQTTPropAdd_UserProp(&propertyBuilder, &userProperty, &(uint8_t){ MQTT_PACKET_TYPE_UNSUBSCRIBE } );

    /* Send UNSUBSCRIBE packet. */
    mqttStatus = MQTT_Unsubscribe( pMqttContext,
                                   pSubscriptionList,
                                   sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                                   globalUnsubscribePacketIdentifier,
                                   &propertyBuilder );

    if( mqttStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send UNSUBSCRIBE packet to broker with error = %s.",
                    MQTT_Status_strerror( mqttStatus ) ) );
    }
    else
    {
        LogDebug( ( "UNSUBSCRIBE sent topic %.*s to broker.",
                    topicFilterLength,
                    pTopicFilter ) );

        /* Process incoming packet from the broker. Acknowledgment for unsubscribe
         * operation ( UNSUBACK ) will be received here. This demo uses
         * MQTT_ProcessLoop to receive packet from network. */
        returnStatus = waitForPacketAck( pMqttContext,
                                         globalUnsubscribePacketIdentifier,
                                         MQTT_PROCESS_LOOP_TIMEOUT_MS );
    }

    return returnStatus;
}
/*-----------------------------------------------------------*/

bool Fleet_PublishToTopic( const char * pTopicFilter,
                     uint16_t topicFilterLength,
                     const char * pPayload,
                     size_t payloadLength )
{
    bool returnStatus = false;
    MQTTStatus_t mqttStatus = MQTTSuccess;
    uint8_t publishIndex = MAX_OUTGOING_PUBLISHES;
    MQTTContext_t * pMqttContext = &mqttContext;

    assert( pMqttContext != NULL );
    assert( pTopicFilter != NULL );
    assert( topicFilterLength > 0 );

    /* Get the next free index for the outgoing publish. All QoS1 outgoing
     * publishes are stored until a PUBACK is received. These messages are
     * stored for supporting a resend if a network connection is broken before
     * receiving a PUBACK. */
    returnStatus = getNextFreeIndexForOutgoingPublishes( &publishIndex );

    if( returnStatus == false )
    {
        LogError( ( "Unable to find a free spot for outgoing PUBLISH message." ) );
    }
    else
    {
        LogDebug( ( "Published payload: %.*s",
                    ( int ) payloadLength,
                    ( const char * ) pPayload ) );

        /* This example publishes to only one topic and uses QOS1. */
        outgoingPublishPackets[ publishIndex ].pubInfo.qos = MQTTQoS1;
        outgoingPublishPackets[ publishIndex ].pubInfo.pTopicName = pTopicFilter;
        outgoingPublishPackets[ publishIndex ].pubInfo.topicNameLength = topicFilterLength;
        outgoingPublishPackets[ publishIndex ].pubInfo.pPayload = pPayload;
        outgoingPublishPackets[ publishIndex ].pubInfo.payloadLength = payloadLength;

        /* Get a new packet id. */
        outgoingPublishPackets[ publishIndex ].packetId = MQTT_GetPacketId( pMqttContext );

        /* MQTTv5: Create property builder to handle MQTTv5 properties */
        MQTTPropBuilder_t propertyBuilder ;
        uint8_t buf[100] ;
        size_t bufLength = sizeof(buf);
        MQTTPropertyBuilder_Init(&propertyBuilder, buf, bufLength) ;

        /* MQTTv5: Add publish properties */                        
        MQTTPropAdd_PayloadFormat(&propertyBuilder, 1, &(uint8_t){ MQTT_PACKET_TYPE_PUBLISH } );
        MQTTPropAdd_TopicAlias(&propertyBuilder, 1, &(uint8_t){ MQTT_PACKET_TYPE_PUBLISH } );

        /* Send PUBLISH packet. */
        mqttStatus = MQTT_Publish( pMqttContext,
                                   &outgoingPublishPackets[ publishIndex ].pubInfo,
                                   outgoingPublishPackets[ publishIndex ].packetId,
                                   &propertyBuilder );

        if( mqttStatus != MQTTSuccess )
        {
            LogError( ( "Failed to send PUBLISH packet to broker with error = %s.",
                        MQTT_Status_strerror( mqttStatus ) ) );
            cleanupOutgoingPublishAt( publishIndex );
            returnStatus = false;
        }
        else
        {
            LogDebug( ( "PUBLISH sent for topic %.*s to broker with packet ID %u.",
                        topicFilterLength,
                        pTopicFilter,
                        outgoingPublishPackets[ publishIndex ].packetId ) );
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

bool ProcessLoopWithTimeout( void )
{
    uint32_t ulMqttProcessLoopTimeoutTime;
    uint32_t ulCurrentTime;

    MQTTStatus_t eMqttStatus = MQTTSuccess;
    bool returnStatus = false;

    ulCurrentTime = mqttContext.getTime();
    ulMqttProcessLoopTimeoutTime = ulCurrentTime + MQTT_PROCESS_LOOP_TIMEOUT_MS;

    /* Call MQTT_ProcessLoop multiple times until the timeout expires or
     * #MQTT_ProcessLoop fails. */
    while( ( ulCurrentTime < ulMqttProcessLoopTimeoutTime ) &&
           ( eMqttStatus == MQTTSuccess || eMqttStatus == MQTTNeedMoreBytes ) )
    {
        eMqttStatus = MQTT_ProcessLoop( &mqttContext );
        ulCurrentTime = mqttContext.getTime();
    }

    if( ( eMqttStatus != MQTTSuccess ) && ( eMqttStatus != MQTTNeedMoreBytes ) )
    {
        LogError( ( "MQTT_ProcessLoop returned with status = %s.",
                    MQTT_Status_strerror( eMqttStatus ) ) );
    }
    else
    {
        LogDebug( ( "MQTT_ProcessLoop successful." ) );
        returnStatus = true;
    }

    return returnStatus;
}
/*-----------------------------------------------------------*/