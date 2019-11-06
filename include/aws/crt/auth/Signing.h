#pragma once
/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <aws/crt/Exports.h>

#include <aws/auth/signing_config.h>

#include <functional>
#include <memory>

namespace Aws
{
    namespace Crt
    {
        namespace Http
        {
            class HttpRequest;
        }

        namespace Auth
        {
            enum class SigningConfigType
            {
                Aws = AWS_SIGNING_CONFIG_AWS
            };

            /**
             * HTTP signing callback.  The second parameter is an aws error code,  The signing was successful
             * iff the error code is AWS_ERROR_SUCCESS.
             */
            using OnHttpRequestSigningComplete =
                std::function<void(const std::shared_ptr<Aws::Crt::Http::HttpRequest> &, int)>;

            /**
             * Base class for all different signing configurations.  Type functions as a
             * primitive RTTI for downcasting.
             */
            class AWS_CRT_CPP_API ISigningConfig
            {
              public:
                ISigningConfig() = default;
                ISigningConfig(const ISigningConfig &) = delete;
                ISigningConfig(ISigningConfig &&) = delete;
                ISigningConfig &operator=(const ISigningConfig &) = delete;
                ISigningConfig &operator=(ISigningConfig &&) = delete;

                virtual ~ISigningConfig() = default;

                /**
                 * RTTI query for the SigningConfig hierarchy
                 */
                virtual SigningConfigType GetType(void) const = 0;
            };

            /**
             * Abstract base for all http request signers.  Synchronous interface.  Intended to
             * be a tight wrapper around aws-c-* signer implementations.
             */
            class AWS_CRT_CPP_API IHttpRequestSigner
            {
              public:
                IHttpRequestSigner() = default;
                IHttpRequestSigner(const IHttpRequestSigner &) = delete;
                IHttpRequestSigner(IHttpRequestSigner &&) = delete;
                IHttpRequestSigner &operator=(const IHttpRequestSigner &) = delete;
                IHttpRequestSigner &operator=(IHttpRequestSigner &&) = delete;

                virtual ~IHttpRequestSigner() = default;

                virtual bool SignRequest(
                    const std::shared_ptr<Aws::Crt::Http::HttpRequest> &request,
                    const std::shared_ptr<ISigningConfig> &config,
                    const OnHttpRequestSigningComplete &completionCallback) = 0;

                /**
                 * Whether or not the signer is in a valid state
                 */
                virtual bool IsValid() const = 0;
            };

        } // namespace Auth
    }     // namespace Crt
} // namespace Aws