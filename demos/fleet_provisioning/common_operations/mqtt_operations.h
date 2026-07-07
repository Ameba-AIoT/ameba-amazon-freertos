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

#ifndef MQTT_OPERATIONS_H_
#define MQTT_OPERATIONS_H_

/* MQTT API header. */
#include "core_mqtt.h"

/* corePKCS11 include. */
#include "core_pkcs11.h"

#include "iot_network.h"

/* Include AWS IoT metrics macros header. */
#include "aws_iot_metrics.h"

/* Include header for connection configurations. */
#include "aws_clientcredential.h"

/* Include header for client credentials. */
#include "aws_clientcredential_keys.h"

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

#include "fleet_provisioning_config.h"

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND                      ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                        ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * These configurations are required. Throw compilation error if the below
 * configs are not defined.
 */
#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

/**
 * @brief The root CA certificate belonging to the broker.
 */
#ifndef democonfigROOT_CA_PEM
    #define democonfigROOT_CA_PEM    tlsATS1_ROOT_CERTIFICATE_PEM
#endif

#ifndef democonfigCLIENT_IDENTIFIER

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 */
    #define democonfigCLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

#ifndef democonfigMQTT_BROKER_PORT

/**
 * @brief The port to use for the demo.
 */
    #define democonfigMQTT_BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

#define democonfigNETWORK_BUFFER_SIZE 5120U

/**
 * @brief Application callback type to handle the incoming publishes.
 *
 * @param[in] pPublishInfo Pointer to publish info of the incoming publish.
 * @param[in] packetIdentifier Packet identifier of the incoming publish.
 */
typedef void (* MQTTPublishCallback_t )( MQTTPublishInfo_t * pPublishInfo,
                                         uint16_t packetIdentifier );

/**
 * @brief Establish a MQTT connection.
 *
 * @param[in] publishCallback The callback function to receive incoming
 * publishes from the MQTT broker.
 * @param[in] p11Session The PKCS #11 session to use.
 * @param[in] pClientCertLabel The client certificate PKCS #11 label to use.
 * @param[in] pPrivateKeyLabel The private key PKCS #11 label for the client certificate.
 *
 * @return true if an MQTT session is established;
 * false otherwise.
 */
bool Fleet_EstablishMqttSession( MQTTPublishCallback_t publishCallback,
                           CK_SESSION_HANDLE p11Session,
                           char * pClientCertLabel,
                           char * pPrivateKeyLabel );

/**
 * @brief Disconnect the MQTT connection.
 *
 * @return true if the MQTT session was successfully disconnected;
 * false otherwise.
 */
bool Fleet_DisconnectMqttSession( void );

/**
 * @brief Subscribe to a MQTT topic filter.
 *
 * @param[in] pTopicFilter The topic filter to subscribe to.
 * @param[in] topicFilterLength Length of the topic buffer.
 *
 * @return true if subscribe operation was successful;
 * false otherwise.
 */
bool Fleet_SubscribeToTopic( const char * pTopicFilter,
                       uint16_t topicFilterLength );

/**
 * @brief Unsubscribe from a MQTT topic filter.
 *
 * @param[in] pTopicFilter The topic filter to unsubscribe from.
 * @param[in] topicFilterLength Length of the topic buffer.
 *
 * @return true if unsubscribe operation was successful;
 * false otherwise.
 */
bool Fleet_UnsubscribeFromTopic( const char * pTopicFilter,
                           uint16_t topicFilterLength );

/**
 * @brief Publish a message to a MQTT topic.
 *
 * @param[in] pTopic The topic to publish the message on.
 * @param[in] topicLength Length of the topic.
 * @param[in] pMessage The message to publish.
 * @param[in] messageLength Length of the message.
 *
 * @return true if PUBLISH was successfully sent;
 * false otherwise.
 */
bool Fleet_PublishToTopic( const char * pTopic,
                     uint16_t topicLength,
                     const char * pMessage,
                     size_t messageLength );

/**
 * @brief Invoke the core MQTT library's process loop function.
 *
 * @return true if process loop was successful;
 * false otherwise.
 */
bool ProcessLoopWithTimeout( void );

/**
 * @brief Override the MQTT client identifier used in the next
 * Fleet_EstablishMqttSession call.  Pass the Thing name received
 * from RegisterThing so the device connects as its registered identity.
 * Passing NULL resets to the compile-time democonfigCLIENT_IDENTIFIER.
 *
 * @param[in] pClientId  Client identifier string (need not be NUL-terminated).
 * @param[in] length     Length of pClientId in bytes.
 */
void Fleet_SetClientIdentifier( const char * pClientId,
                                uint16_t length );


#endif /* ifndef MQTT_OPERATIONS_H_ */