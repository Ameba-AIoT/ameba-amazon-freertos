/*
 * AWS IoT Device SDK for Embedded C 202412.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Demo for showing use of the Fleet Provisioning library to use the Fleet
 * Provisioning feature of AWS IoT Core for provisioning devices with
 * credentials. This demo shows how a device can be provisioned with AWS IoT
 * Core using the Certificate Signing Request workflow of the Fleet
 * Provisioning feature.
 *
 * The Fleet Provisioning library provides macros and helper functions for
 * assembling MQTT topics strings, and for determining whether an incoming MQTT
 * message is related to the Fleet Provisioning API of AWS IoT Core. The Fleet
 * Provisioning library does not depend on any particular MQTT library,
 * therefore the functionality for MQTT operations is placed in another file
 * (mqtt_operations.c). This demo uses the coreMQTT library. If needed,
 * mqtt_operations.c can be modified to replace coreMQTT with another MQTT
 * library. This demo requires using the AWS IoT Core broker as Fleet
 * Provisioning is an AWS IoT Core feature.
 *
 * This demo provisions a device certificate using the provisioning by claim
 * workflow with a Keys and Certificate Request. The demo connects to AWS
 * IoT Core using provided claim credentials (whose certificate needs to be
 * registered with IoT Core before running this demo), subscribes to the
 * CreateKeysAndCertificate topics, and obtains keys and certificate. It then
 * subscribes to the RegisterThing topics and activates the certificate and
 * obtains a Thing using the provisioning template. Finally, it reconnects to
 * AWS IoT Core using the new credentials.
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* corePKCS11 includes. */
#include "core_pkcs11.h"
#include "core_pkcs11_config.h"

/* AWS IoT Fleet Provisioning Library. */
#include "fleet_provisioning.h"

/* Demo includes. */
#include "mqtt_operations.h"
#include "pkcs11_operations.h"
#include "fleet_provisioning_serializer.h"

#include "aws_demo.h"

/**
 * These configurations are required. Throw compilation error if it is not
 * defined.
 */
#ifndef PROVISIONING_TEMPLATE_NAME
    #define PROVISIONING_TEMPLATE_NAME clientcredential_IOT_PROVISIONING_TEMPLATE_NAME
#endif
#ifndef DEVICE_SERIAL_NUMBER
    #define DEVICE_SERIAL_NUMBER clientcredentialIOT_SERIAL_NUMBER
#endif

/**
 * @brief The length of #PROVISIONING_TEMPLATE_NAME.
 */
#define PROVISIONING_TEMPLATE_NAME_LENGTH    ( ( uint16_t ) ( sizeof( PROVISIONING_TEMPLATE_NAME ) - 1 ) )

/**
 * @brief The length of #DEVICE_SERIAL_NUMBER.
 */
#define DEVICE_SERIAL_NUMBER_LENGTH          ( ( uint16_t ) ( sizeof( DEVICE_SERIAL_NUMBER ) - 1 ) )

/**
 * @brief Size of AWS IoT Thing name buffer.
 *
 * See https://docs.aws.amazon.com/iot/latest/apireference/API_CreateThing.html#iot-CreateThing-request-thingName
 */
#define MAX_THING_NAME_LENGTH                128

/**
 * @brief The maximum number of times to run the loop in this demo.
 *
 * @note The demo loop is attempted to re-run only if it fails in an iteration.
 * Once the demo loop succeeds in an iteration, the demo exits successfully.
 */
#ifndef FLEET_PROV_MAX_DEMO_LOOP_COUNT
    #define FLEET_PROV_MAX_DEMO_LOOP_COUNT    ( 3 )
#endif

/**
 * @brief Time in milliseconds to wait between retries of the demo loop if
 * demo loop fails.
 */
#define DELAY_BETWEEN_DEMO_RETRY_ITERATIONS_MS    ( 5000U )

/**
 * @brief Size of buffer in which to hold the private key.
 */
#define PRIV_KEY_BUFFER_LENGTH                         2048

/**
 * @brief Size of buffer in which to hold the certificate.
 */
#define CERT_BUFFER_LENGTH                             2048

/**
 * @brief Size of buffer in which to hold the certificate id.
 *
 * See https://docs.aws.amazon.com/iot/latest/apireference/API_Certificate.html#iot-Type-Certificate-certificateId
 */
#define CERT_ID_BUFFER_LENGTH                          64

/**
 * @brief Size of buffer in which to hold the certificate ownership token.
 */
#define OWNERSHIP_TOKEN_BUFFER_LENGTH                  512

/**
 * @brief Status values of the Fleet Provisioning response.
 */
typedef enum
{
    ResponseNotReceived,
    ResponseAccepted,
    ResponseRejected
} ResponseStatus_t;

/*-----------------------------------------------------------*/

/**
 * @brief Status reported from the MQTT publish callback.
 */
static ResponseStatus_t responseStatus;

/**
 * @brief Buffer to hold the provisioned AWS IoT Thing name.
 */
static char thingName[ MAX_THING_NAME_LENGTH ];

/**
 * @brief Length of the AWS IoT Thing name.
 */
static size_t thingNameLength;

/**
 * @brief Buffer to hold responses received from the AWS IoT Fleet Provisioning
 * APIs. When the MQTT publish callback receives an expected Fleet Provisioning
 * accepted payload, it copies it into this buffer.
 */
static uint8_t payloadBuffer[ democonfigNETWORK_BUFFER_SIZE ];

/**
 * @brief Length of the payload stored in #payloadBuffer. This is set by the
 * MQTT publish callback when it copies a received payload into #payloadBuffer.
 */
static size_t payloadLength;

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/*-----------------------------------------------------------*/

/**
 * @brief Callback to receive the incoming publish messages from the MQTT
 * broker. Sets responseStatus if an expected CreateKeysAndCertificate or
 * RegisterThing response is received, and copies the response into
 * responseBuffer if the response is an accepted one.
 *
 * @param[in] pPublishInfo Pointer to publish info of the incoming publish.
 * @param[in] packetIdentifier Packet identifier of the incoming publish.
 */
static void provisioningPublishCallback( MQTTPublishInfo_t * pPublishInfo,
                                         uint16_t packetIdentifier );

/**
 * @brief Run the MQTT process loop to get a response.
 */
static bool waitForResponse( void );

/**
 * @brief Subscribe to the CreateKeysAndCertificate accepted and rejected topics.
 */
static bool subscribeToKeysCertResponseTopics( void );

/**
 * @brief Unsubscribe from the CreateKeysAndCertificate accepted and rejected topics.
 */
static bool unsubscribeFromKeysCertResponseTopics( void );

/**
 * @brief Subscribe to the RegisterThing accepted and rejected topics.
 */
static bool subscribeToRegisterThingResponseTopics( void );

/**
 * @brief Unsubscribe from the RegisterThing accepted and rejected topics.
 */
static bool unsubscribeFromRegisterThingResponseTopics( void );

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


static void provisioningPublishCallback( MQTTPublishInfo_t * pPublishInfo,
                                         uint16_t packetIdentifier )
{
    FleetProvisioningStatus_t status;
    FleetProvisioningTopic_t api;
    const char * cborDump;

    /* Silence compiler warnings about unused variables. */
    ( void ) packetIdentifier;

    status = FleetProvisioning_MatchTopic( pPublishInfo->pTopicName,
                                           pPublishInfo->topicNameLength, &api );

    if( status != FleetProvisioningSuccess )
    {
        LogWarn( ( "Unexpected publish message received. Topic: %.*s.",
                   ( int ) pPublishInfo->topicNameLength,
                   ( const char * ) pPublishInfo->pTopicName ) );
    }
    else
    {
        if( api == FleetProvCborCreateKeysAndCertAccepted )
        {
            LogInfo( ( "Received accepted response from Fleet Provisioning CreateKeysAndCertificate API." ) );

            cborDump = getStringFromCbor( ( const uint8_t * ) pPublishInfo->pPayload, pPublishInfo->payloadLength );
            LogDebug( ( "Payload: %s", cborDump ) );
            free( ( void * ) cborDump );

            responseStatus = ResponseAccepted;

            /* Copy the payload from the MQTT library's buffer to #payloadBuffer. */
            ( void ) memcpy( ( void * ) payloadBuffer,
                             ( const void * ) pPublishInfo->pPayload,
                             ( size_t ) pPublishInfo->payloadLength );

            payloadLength = pPublishInfo->payloadLength;
        }
        else if( api == FleetProvCborCreateKeysAndCertRejected )
        {
            LogError( ( "Received rejected response from Fleet Provisioning CreateKeysAndCertificate API." ) );

            cborDump = getStringFromCbor( ( const uint8_t * ) pPublishInfo->pPayload, pPublishInfo->payloadLength );
            LogError( ( "Payload: %s", cborDump ) );
            free( ( void * ) cborDump );

            responseStatus = ResponseRejected;
        }
        else if( api == FleetProvCborRegisterThingAccepted )
        {
            LogInfo( ( "Received accepted response from Fleet Provisioning RegisterThing API." ) );

            cborDump = getStringFromCbor( ( const uint8_t * ) pPublishInfo->pPayload, pPublishInfo->payloadLength );
            LogDebug( ( "Payload: %s", cborDump ) );
            free( ( void * ) cborDump );

            responseStatus = ResponseAccepted;

            /* Copy the payload from the MQTT library's buffer to #payloadBuffer. */
            ( void ) memcpy( ( void * ) payloadBuffer,
                             ( const void * ) pPublishInfo->pPayload,
                             ( size_t ) pPublishInfo->payloadLength );

            payloadLength = pPublishInfo->payloadLength;
        }
        else if( api == FleetProvCborRegisterThingRejected )
        {
            LogError( ( "Received rejected response from Fleet Provisioning RegisterThing API." ) );

            cborDump = getStringFromCbor( ( const uint8_t * ) pPublishInfo->pPayload, pPublishInfo->payloadLength );
            LogError( ( "Payload: %s", cborDump ) );
            free( ( void * ) cborDump );

            responseStatus = ResponseRejected;
        }
        else
        {
            LogError( ( "Received message on unexpected Fleet Provisioning topic. Topic: %.*s.",
                        ( int ) pPublishInfo->topicNameLength,
                        ( const char * ) pPublishInfo->pTopicName ) );
        }
    }
}
/*-----------------------------------------------------------*/

static bool waitForResponse( void )
{
    bool status = false;

    responseStatus = ResponseNotReceived;

    /* responseStatus is updated from the MQTT publish callback. */
    ( void ) ProcessLoopWithTimeout();

    if( responseStatus == ResponseNotReceived )
    {
        LogError( ( "Timed out waiting for response." ) );
    }

    if( responseStatus == ResponseAccepted )
    {
        status = true;
    }

    return status;
}
/*-----------------------------------------------------------*/

static bool subscribeToKeysCertResponseTopics( void )
{
    bool status;

    status = Fleet_SubscribeToTopic( FP_CBOR_CREATE_KEYS_ACCEPTED_TOPIC,
                               FP_CBOR_CREATE_KEYS_ACCEPTED_LENGTH );

    if( status == false )
    {
        LogError( ( "Failed to subscribe to fleet provisioning topic: %.*s.",
                    FP_CBOR_CREATE_KEYS_ACCEPTED_LENGTH,
                    FP_CBOR_CREATE_KEYS_ACCEPTED_TOPIC ) );
    }

    if( status == true )
    {
        status = Fleet_SubscribeToTopic( FP_CBOR_CREATE_KEYS_REJECTED_TOPIC,
                                   FP_CBOR_CREATE_KEYS_REJECTED_LENGTH );

        if( status == false )
        {
            LogError( ( "Failed to subscribe to fleet provisioning topic: %.*s.",
                        FP_CBOR_CREATE_KEYS_REJECTED_LENGTH,
                        FP_CBOR_CREATE_KEYS_REJECTED_TOPIC ) );
        }
    }

    return status;
}
/*-----------------------------------------------------------*/

static bool unsubscribeFromKeysCertResponseTopics( void )
{
    bool status;

    status = Fleet_UnsubscribeFromTopic( FP_CBOR_CREATE_KEYS_ACCEPTED_TOPIC,
                                   FP_CBOR_CREATE_KEYS_ACCEPTED_LENGTH );

    if( status == false )
    {
        LogError( ( "Failed to unsubscribe from fleet provisioning topic: %.*s.",
                    FP_CBOR_CREATE_KEYS_ACCEPTED_LENGTH,
                    FP_CBOR_CREATE_KEYS_ACCEPTED_TOPIC ) );
    }

    if( status == true )
    {
        status = Fleet_UnsubscribeFromTopic( FP_CBOR_CREATE_KEYS_REJECTED_TOPIC,
                                       FP_CBOR_CREATE_KEYS_REJECTED_LENGTH );

        if( status == false )
        {
            LogError( ( "Failed to unsubscribe from fleet provisioning topic: %.*s.",
                        FP_CBOR_CREATE_KEYS_REJECTED_LENGTH,
                        FP_CBOR_CREATE_KEYS_REJECTED_TOPIC ) );
        }
    }

    return status;
}
/*-----------------------------------------------------------*/

static bool subscribeToRegisterThingResponseTopics( void )
{
    bool status;

    status = Fleet_SubscribeToTopic( FP_CBOR_REGISTER_ACCEPTED_TOPIC( PROVISIONING_TEMPLATE_NAME ),
                               FP_CBOR_REGISTER_ACCEPTED_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ) );

    if( status == false )
    {
        LogError( ( "Failed to subscribe to fleet provisioning topic: %.*s.",
                    FP_CBOR_REGISTER_ACCEPTED_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ),
                    FP_CBOR_REGISTER_ACCEPTED_TOPIC( PROVISIONING_TEMPLATE_NAME ) ) );
    }

    if( status == true )
    {
        status = Fleet_SubscribeToTopic( FP_CBOR_REGISTER_REJECTED_TOPIC( PROVISIONING_TEMPLATE_NAME ),
                                   FP_CBOR_REGISTER_REJECTED_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ) );

        if( status == false )
        {
            LogError( ( "Failed to subscribe to fleet provisioning topic: %.*s.",
                        FP_CBOR_REGISTER_REJECTED_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ),
                        FP_CBOR_REGISTER_REJECTED_TOPIC( PROVISIONING_TEMPLATE_NAME ) ) );
        }
    }

    return status;
}
/*-----------------------------------------------------------*/

static bool unsubscribeFromRegisterThingResponseTopics( void )
{
    bool status;

    status = Fleet_UnsubscribeFromTopic( FP_CBOR_REGISTER_ACCEPTED_TOPIC( PROVISIONING_TEMPLATE_NAME ),
                                   FP_CBOR_REGISTER_ACCEPTED_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ) );

    if( status == false )
    {
        LogError( ( "Failed to unsubscribe from fleet provisioning topic: %.*s.",
                    FP_CBOR_REGISTER_ACCEPTED_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ),
                    FP_CBOR_REGISTER_ACCEPTED_TOPIC( PROVISIONING_TEMPLATE_NAME ) ) );
    }

    if( status == true )
    {
        status = Fleet_UnsubscribeFromTopic( FP_CBOR_REGISTER_REJECTED_TOPIC( PROVISIONING_TEMPLATE_NAME ),
                                       FP_CBOR_REGISTER_REJECTED_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ) );

        if( status == false )
        {
            LogError( ( "Failed to unsubscribe from fleet provisioning topic: %.*s.",
                        FP_CBOR_REGISTER_REJECTED_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ),
                        FP_CBOR_REGISTER_REJECTED_TOPIC( PROVISIONING_TEMPLATE_NAME ) ) );
        }
    }

    return status;
}
/*-----------------------------------------------------------*/

/* This example uses a single application task, which shows that how to use
 * the Fleet Provisioning library to generate and validate AWS IoT Fleet
 * Provisioning MQTT topics, and use the coreMQTT library to communicate with
 * the AWS IoT Fleet Provisioning APIs. */
int RunFleetProvisioningKeysCertDemo( bool awsIotMqttMode,
                               const char * pIdentifier,
                               void * pNetworkServerInfo,
                               void * pNetworkCredentialInfo,
                               const IotNetworkInterface_t * pNetworkInterface )
{
    bool status = false;
    /* Buffer for holding received certificate until it is saved. */
    char certificate[ CERT_BUFFER_LENGTH ];
    size_t certificateLength;
    /* Buffer for holding received private key until it is saved. */
    char privatekey[ PRIV_KEY_BUFFER_LENGTH ];
    size_t privatekeyLength;
    /* Buffer for holding the certificate ID. */
    char certificateId[ CERT_ID_BUFFER_LENGTH ];
    size_t certificateIdLength;
    /* Buffer for holding the certificate ownership token. */
    char ownershipToken[ OWNERSHIP_TOKEN_BUFFER_LENGTH ];
    size_t ownershipTokenLength;
    bool connectionEstablished = false;
    CK_SESSION_HANDLE p11Session;
    int demoRunCount = 0;
    CK_RV pkcs11ret = CKR_OK;
    bool alreadyProvisioned = false;
    size_t persistedThingNameLength = 0;

    do
    {
        /* Wait for Networking */
        RTK_SDK_CHECK_CONNECTIVITY();

        /* Check if the device has already been provisioned by looking for a
        * persisted Thing name and a valid device certificate in flash. */
       if( ( PKCS11_PAL_FindObject(
                 ( CK_BYTE_PTR ) pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                 sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) - 1 )
             != 0 )
           && readFile( pkcs11configLABEL_THING_NAME,
                        thingName, sizeof( thingName ),
                        &persistedThingNameLength ) )
        {
            thingNameLength = persistedThingNameLength;
            LogInfo( ( "Device already provisioned as \"%.*s\" — skipping claim flow.",
                        ( int ) thingNameLength, thingName ) );
            alreadyProvisioned = true;
        }

        if( alreadyProvisioned )
        {
            pkcs11ret = xInitializePkcs11Session( &p11Session );
            if( pkcs11ret != CKR_OK )
            {
                LogError( ( "Failed to initialize PKCS #11." ) );
                status = false;
            }
            else
            {
                /* Re-register device cert and key into this PKCS #11 session.
                 * They are stored as PEM in flash; loadDeviceCredentials registers
                 * them with full attribute info so prvInitializeClientCredential
                 * can find them by label and read CKA_KEY_TYPE correctly. */
                status = loadDeviceCredentials( p11Session );
                if( !status ) 
                {
                    LogError( ( "Failed to load device credentials from flash." ) );
                } 
                else 
                {
                    LogInfo( ( "Loaded device credentials from flash!" ) );
                }

                if( status != false )
                {
                    Fleet_SetClientIdentifier( thingName, ( uint16_t ) thingNameLength );
                    status = Fleet_EstablishMqttSession( provisioningPublishCallback,
                                                        p11Session,
                                                        pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                                        pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS );
                    if( !status ) 
                    {
                        LogError( ( "Failed to connect to MQTT with device credentials" ) );
                    } 
                    else 
                    {
                        LogInfo( ( "Successfully connected to MQTT with device credentials!" ) );
                        status = Fleet_PublishToTopic( "test/hello",
                                     strlen("test/hello"),
                                     "this is a test",
                                     strlen("this is a test") );

                        if( status == false )
                        {
                            LogError( ( "Failed to publish to test/topic!" ) );
                        }
                    }
                }

                /* Skip the full provisioning flow below. */
                goto cleanup;
            }
        }

        /* Initialize the buffer lengths to their max lengths. */
        certificateLength = CERT_BUFFER_LENGTH;
        privatekeyLength = PRIV_KEY_BUFFER_LENGTH;
        certificateIdLength = CERT_ID_BUFFER_LENGTH;
        ownershipTokenLength = OWNERSHIP_TOKEN_BUFFER_LENGTH;

        /* Initialize the PKCS #11 module */
        pkcs11ret = xInitializePkcs11Session( &p11Session );

        if( pkcs11ret != CKR_OK )
        {
            LogError( ( "Failed to initialize PKCS #11." ) );
            status = false;
        }
        else
        {
            /* Insert the claim credentials into the PKCS #11 module */
            status = loadClaimCredentials( p11Session,
                                           NULL,
                                           pkcs11configLABEL_CLAIM_CERTIFICATE,
                                           NULL,
                                           pkcs11configLABEL_CLAIM_PRIVATE_KEY );

            if( status == false )
            {
                LogError( ( "Failed to provision PKCS #11 with claim credentials." ) );
            }
        }

        /**** Connect to AWS IoT Core with provisioning claim credentials *****/

        /* We first use the claim credentials to connect to the broker. These
         * credentials should allow use of the RegisterThing API and one of the
         * CreateCertificatefromCsr or CreateKeysAndCertificate.
         * In this demo we use CreateKeysAndCertificate. */

        if( status == true )
        {
            /* Attempts to connect to the AWS IoT MQTT broker. If the
             * connection fails, retries after a timeout. Timeout value will
             * exponentially increase until maximum attempts are reached. */
            LogInfo( ( "Establishing MQTT session with claim certificate..." ) );
            status = Fleet_EstablishMqttSession( provisioningPublishCallback,
                                           p11Session,
                                           pkcs11configLABEL_CLAIM_CERTIFICATE,
                                           pkcs11configLABEL_CLAIM_PRIVATE_KEY );

            if( status == false )
            {
                LogError( ( "Failed to establish MQTT session." ) );
            }
            else
            {
                LogInfo( ( "Established connection with claim credentials." ) );
                connectionEstablished = true;
            }
        }

        /**** Call the CreateKeysAndCertificate API ***************************/

        /* We use the CreateKeysAndCertificate API to obtain a client certificate
         * and private key. */
        if( status == true )
        {
            /* Subscribe to the CreateKeysAndCertificate accepted and rejected
             * topics. In this demo we use CBOR encoding for the payloads,
             * so we use the CBOR variants of the topics. */
            status = subscribeToKeysCertResponseTopics();
        }

        if( status == true )
        {
            /* Publish an empty payload to the CreateKeysAndCertificate API. */
            status = Fleet_PublishToTopic( FP_CBOR_CREATE_KEYS_PUBLISH_TOPIC,
                                     FP_CBOR_CREATE_KEYS_PUBLISH_LENGTH,
                                     "",
                                     0 );

            if( status == false )
            {
                LogError( ( "Failed to publish to fleet provisioning topic: %.*s.",
                            FP_CBOR_CREATE_KEYS_PUBLISH_LENGTH,
                            FP_CBOR_CREATE_KEYS_PUBLISH_TOPIC ) );
            }
        }

        if( status == true )
        {
            /* Get the response to the CreateKeysAndCertificate request. */
            status = waitForResponse();
        }

        if( status == true )
        {
            /* From the response, extract the certificate, certificate ID, and
             * certificate ownership token. */
            status = parseKeyCertResponse( payloadBuffer,
                                           payloadLength,
                                           certificate,
                                           &certificateLength,
                                           privatekey,
                                           &privatekeyLength,
                                           certificateId,
                                           &certificateIdLength,
                                           ownershipToken,
                                           &ownershipTokenLength );

            if( status == true )
            {
                LogInfo( ( "Received privatekey and certificate with Id: %.*s", ( int ) certificateIdLength, certificateId ) );
            }
        }

        if( status == true )
        {
            /* Save the private key into PKCS #11. */
            status = loadPrivateKey( p11Session,
                                     privatekey,
                                     pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                     privatekeyLength );
        }

        if( status == true )
        {
            /* Save the certificate into PKCS #11. */
            status = loadCertificate( p11Session,
                                      certificate,
                                      pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                      certificateLength );
        }

        if( status == true )
        {
            /* Unsubscribe from the CreateKeysAndCertificate topics. */
            status = unsubscribeFromKeysCertResponseTopics();
        }

        /**** Call the RegisterThing API **************************************/

        /* We then use the RegisterThing API to activate the received certificate,
         * provision AWS IoT resources according to the provisioning template, and
         * receive device configuration. */
        if( status == true )
        {
            /* Create the request payload to publish to the RegisterThing API. */
            status = generateRegisterThingRequest( payloadBuffer,
                                                   democonfigNETWORK_BUFFER_SIZE,
                                                   ownershipToken,
                                                   ownershipTokenLength,
                                                   DEVICE_SERIAL_NUMBER,
                                                   DEVICE_SERIAL_NUMBER_LENGTH,
                                                   &payloadLength );
        }

        if( status == true )
        {
            /* Subscribe to the RegisterThing response topics. */
            status = subscribeToRegisterThingResponseTopics();
        }

        if( status == true )
        {
            /* Publish the RegisterThing request. */
            status = Fleet_PublishToTopic( FP_CBOR_REGISTER_PUBLISH_TOPIC( PROVISIONING_TEMPLATE_NAME ),
                                     FP_CBOR_REGISTER_PUBLISH_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ),
                                     ( char * ) payloadBuffer,
                                     payloadLength );

            if( status == false )
            {
                LogError( ( "Failed to publish to fleet provisioning topic: %.*s.",
                            FP_CBOR_REGISTER_PUBLISH_LENGTH( PROVISIONING_TEMPLATE_NAME_LENGTH ),
                            FP_CBOR_REGISTER_PUBLISH_TOPIC( PROVISIONING_TEMPLATE_NAME ) ) );
            }
        }

        if( status == true )
        {
            /* Get the response to the RegisterThing request. */
            status = waitForResponse();
        }

        if( status == true )
        {
            /* Extract the Thing name from the response. */
            thingNameLength = MAX_THING_NAME_LENGTH;
            status = parseRegisterThingResponse( payloadBuffer,
                                                 payloadLength,
                                                 thingName,
                                                 &thingNameLength );

            if( status == true )
            {
                LogInfo( ( "Received AWS IoT Thing name: %.*s", ( int ) thingNameLength, thingName ) );
            
                /* Persist the Thing name so it survives reboot and is used
                 * as the MQTT client ID on subsequent connections. */
                if( !writeFile( pkcs11configLABEL_THING_NAME,
                                thingName, thingNameLength ) )
                {
                    LogError( ( "Failed to persist Thing name to flash." ) );
                    status = false;
                }
            }
        }

        if( status == true )
        {
            /* Unsubscribe from the RegisterThing topics. */
            unsubscribeFromRegisterThingResponseTopics();
        }

        /**** Disconnect from AWS IoT Core ************************************/

        /* As we have completed the provisioning workflow, we disconnect from
         * the connection using the provisioning claim credentials. We will
         * establish a new MQTT connection with the newly provisioned
         * credentials. */
        if( connectionEstablished == true )
        {
            Fleet_DisconnectMqttSession();
            connectionEstablished = false;
        }

        /**** Connect to AWS IoT Core with provisioned certificate ************/

        if( status == true )
        {
            LogInfo( ( "Establishing MQTT session with provisioned certificate..." ) );
            /* Set the thingname received from AWS provisioning flow */
            Fleet_SetClientIdentifier( thingName, ( uint16_t ) thingNameLength );
            status = Fleet_EstablishMqttSession( provisioningPublishCallback,
                                           p11Session,
                                           pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                           pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS );

            if( status != true )
            {
                LogError( ( "Failed to establish MQTT session with provisioned "
                            "credentials. Verify on your AWS account that the "
                            "new certificate is active and has an attached IoT "
                            "Policy that allows the \"iot:Connect\" action." ) );
            }
            else
            {
                LogInfo( ( "Successfully established connection with provisioned credentials." ) );
                connectionEstablished = true;
            }
        }

        /**** Finish **********************************************************/

        if( connectionEstablished == true )
        {
            /* Close the connection. */
            Fleet_DisconnectMqttSession();
            connectionEstablished = false;
        }

cleanup:
        pkcs11CloseSession( p11Session );

        /**** Retry in case of failure ****************************************/

        /* Increment the demo run count. */
        demoRunCount++;

        if( status == true )
        {
            LogInfo( ( "Demo iteration %d is successful.", demoRunCount ) );
        }
        /* Attempt to retry a failed iteration of demo for up to #FLEET_PROV_MAX_DEMO_LOOP_COUNT times. */
        else if( demoRunCount < FLEET_PROV_MAX_DEMO_LOOP_COUNT )
        {
            LogWarn( ( "Demo iteration %d failed. Retrying...", demoRunCount ) );
            vTaskDelay( pdMS_TO_TICKS( DELAY_BETWEEN_DEMO_RETRY_ITERATIONS_MS ) );
        }
        /* Failed all #FLEET_PROV_MAX_DEMO_LOOP_COUNT demo iterations. */
        else
        {
            LogError( ( "All %d demo iterations failed.", FLEET_PROV_MAX_DEMO_LOOP_COUNT ) );
            break;
        }
    } while( status != true );

    /* Log demo success. */
    if( status == true )
    {
        LogInfo( ( "Demo completed successfully." ) );

        /* Deliberately do NOT write the PEM cert/key to flash here.
         *
         * DOWNLOADED_CERT_LABEL / DOWNLOADED_PRIVATE_KEY_LABEL resolve to the
         * device cert/key labels, and writeFile() shares those flash slots with the
         * PKCS #11 PAL. loadPrivateKey()/loadCertificate() above already stored the
         * DER PKCS #11 objects there -- the form the TLS stack reads back via
         * C_GetAttributeValue()/C_Sign(). Writing PEM over them left every other
         * example unusable (they resolve the same labels by default in iot_tls.c)
         * until a later run converted the slots back to DER.
         *
         * The DER objects persist across reboot on their own; only the Thing name
         * needs explicit persistence (done earlier via writeFile). */
    }

    return ( status == true ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*-----------------------------------------------------------*/