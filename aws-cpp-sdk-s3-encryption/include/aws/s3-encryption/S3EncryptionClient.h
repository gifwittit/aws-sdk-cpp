/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <aws/s3-encryption/s3Encryption_EXPORTS.h>
#include <aws/s3/S3Client.h>
#include <aws/s3-encryption/modules/CryptoModuleFactory.h>
#include <aws/core/client/AWSError.h>

namespace Aws
{
    namespace S3Encryption
    {
        struct AWS_S3ENCRYPTION_API S3EncryptionErrors
        {
            S3EncryptionErrors() = default;
            S3EncryptionErrors(const Aws::Utils::Crypto::CryptoErrors& error) :cryptoError(error), m_isS3Error(false) {}
            S3EncryptionErrors(const Aws::S3::S3Errors& error) :s3Error(error), m_isS3Error(true) {}

            inline bool IsS3Error() const { return m_isS3Error; }
            inline bool IsCryptoError() const { return !m_isS3Error; }

            union {
                Aws::Utils::Crypto::CryptoErrors cryptoError;
                Aws::S3::S3Errors s3Error;
            };
        private:
            bool m_isS3Error;
        };

        template<typename ERROR_TYPE>
        Aws::Client::AWSError<S3EncryptionErrors> BuildS3EncryptionError(const Aws::Client::AWSError<ERROR_TYPE>& error)
        {
            Aws::Client::AWSError<S3EncryptionErrors> s3EncryptionError = Aws::Client::AWSError<S3EncryptionErrors>(error.GetErrorType(), error.GetExceptionName(), error.GetMessage(), error.ShouldRetry());
            s3EncryptionError.SetResponseCode(error.GetResponseCode());
            s3EncryptionError.SetResponseHeaders(error.GetResponseHeaders());
            return s3EncryptionError;
        }

        typedef Aws::Utils::Outcome<Aws::S3::Model::PutObjectResult, Aws::Client::AWSError<S3EncryptionErrors>> S3EncryptionPutObjectOutcome;
        typedef Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::Client::AWSError<S3EncryptionErrors>> S3EncryptionGetObjectOutcome;

        class AWS_S3ENCRYPTION_API S3EncryptionClient
        {
        public:
            /*
            * Initialize the S3 Encryption Client with encryption materials, crypto configuration, and a client configuration. If no client configuration is supplied,
            * the default client configuration will be used.
            */
            S3EncryptionClient(const std::shared_ptr<Aws::Utils::Crypto::EncryptionMaterials>& encryptionMaterials, const Aws::S3Encryption::CryptoConfiguration& cryptoConfig,
                const Aws::Client::ClientConfiguration& clientConfiguration = Aws::Client::ClientConfiguration());

            /*
            * Initialize the S3 Encryption Client with encryption materials, crypto configuration, AWS credentials and a client configuration. If no client configuration is supplied,
            * the default client configuration will be used.
            */
            S3EncryptionClient(const std::shared_ptr<Aws::Utils::Crypto::EncryptionMaterials>& encryptionMaterials, const Aws::S3Encryption::CryptoConfiguration& cryptoConfig,
                const Aws::Auth::AWSCredentials& credentials, const Aws::Client::ClientConfiguration& clientConfiguration = Aws::Client::ClientConfiguration());

            /*
            * Initialize the S3 Encryption Client with encryption materials, crypto configuration, AWS credentials provider and a client configuration. If no client configuration is supplied,
            * the default client configuration will be used.
            */
            S3EncryptionClient(const std::shared_ptr<Aws::Utils::Crypto::EncryptionMaterials>& encryptionMaterials, const Aws::S3Encryption::CryptoConfiguration& cryptoConfig,
                const std::shared_ptr<Aws::Auth::AWSCredentialsProvider>& credentialsProvider, const Aws::Client::ClientConfiguration& clientConfiguration = Aws::Client::ClientConfiguration());

            S3EncryptionClient(const S3EncryptionClient&) = delete;
            S3EncryptionClient& operator=(const S3EncryptionClient&) = delete;

            /*
            * Function to put an object encrypted to S3.
            */
            S3EncryptionPutObjectOutcome PutObject(const Aws::S3::Model::PutObjectRequest& request) const;

            /*
            * Function to get an object decrypted from S3.
            */
            S3EncryptionGetObjectOutcome GetObject(const Aws::S3::Model::GetObjectRequest& request) const;

            inline bool MultipartUploadSupported() const { return false; }

        private:
            /*
            * Function to get the instruction file object of a encrypted object from S3. This instruction file object will be used to assist decryption.
            */
            Aws::S3::Model::GetObjectOutcome GetInstructionFileObject(const Aws::S3::Model::GetObjectRequest& originalGetRequest) const;

            Aws::UniquePtr<Aws::S3::S3Client> m_s3Client;
            Aws::S3Encryption::Modules::CryptoModuleFactory m_cryptoModuleFactory;
            std::shared_ptr<Aws::Utils::Crypto::EncryptionMaterials> m_encryptionMaterials;
            const Aws::S3Encryption::CryptoConfiguration m_cryptoConfig;
        };
    }
}
