/*
 * Copyright 2010-2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
#pragma once

#include <aws/common/condition_variable.h>
#include <aws/common/mutex.h>
#include <aws/crt/DateTime.h>
#include <aws/crt/auth/Sigv4Signing.h>
#include <aws/crt/http/HttpConnectionManager.h>
#include <aws/crt/io/Stream.h>
#include <aws/s3/s3_client.h>

#include <queue>
#include <set>

#include "TransferState.h"

class CanaryApp;
struct aws_allocator;
struct aws_event_loop;

namespace Aws
{
    namespace Crt
    {
        namespace Io
        {
            class EndPointMonitorManager;
        }
    } // namespace Crt
} // namespace Aws

/*
 * Makes available a handful of S3 operations invoked by using the REST API to a specific bucket.  Also had
 * functionality for resolving a number of DNS addresses that transfers can be distributed across.
 */
class S3ObjectTransport
{
  public:
    S3ObjectTransport(
        CanaryApp &canaryApp,
        const Aws::Crt::String &bucket,
        uint32_t maxConnections,
        uint64_t minThroughputBytesPerSecond = 0ULL);
    ~S3ObjectTransport();

    /*
     * Returns the endpoint of the bucket being used.
     */
    const Aws::Crt::String &GetEndpoint() const { return m_endpoint; }

    /*
     * Upload a single part object, or a part of an object.
     */
    void PutObject(const std::shared_ptr<TransferState> &transferState, const Aws::Crt::String &key);

    /*
     * Get a single part object, or a part of an object.
     */
    void GetObject(const std::shared_ptr<TransferState> &transferState, const Aws::Crt::String &key);

    /*
     * Given a number of transfers, resolve the appropriate amount of DNS addresses.
     */
    void WarmDNSCache(uint32_t numTransfers);

    /*
    const std::shared_ptr<Aws::Crt::Io::EndPointMonitorManager> &GetEndPointMonitorManager()
    {
        return m_endPointMonitorManager;
    }
    */

  private:
    CanaryApp &m_canaryApp;
    const Aws::Crt::String m_bucketName;
    Aws::Crt::Http::HttpHeader m_hostHeader;
    Aws::Crt::Http::HttpHeader m_contentTypeHeader;
    Aws::Crt::String m_endpoint;

    struct aws_s3_client *m_client;

    // std::shared_ptr<Aws::Crt::Io::EndPointMonitorManager> m_endPointMonitorManager;

    void AddContentLengthHeader(
        const std::shared_ptr<Aws::Crt::Http::HttpRequest> &request,
        const std::shared_ptr<Aws::Crt::Io::InputStream> &body);

    void SendMetaRequest(
        const std::shared_ptr<TransferState> &transferState,
        enum aws_s3_meta_request_type meta_request_type,
        struct aws_http_message *message);
};
