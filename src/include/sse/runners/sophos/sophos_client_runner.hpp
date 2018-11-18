//
// Sophos - Forward Private Searchable Encryption
// Copyright (C) 2016 Raphael Bost
//
// This file is part of Sophos.
//
// Sophos is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// Sophos is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with Sophos.  If not, see <http://www.gnu.org/licenses/>.
//


#pragma once

#include <sse/schemes/sophos/sophos_client.hpp>

#include <google/protobuf/empty.pb.h> // For ::google::protobuf::Empty
#include <grpc++/channel.h>
#include <grpc++/completion_queue.h>

#include <condition_variable>

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

namespace sse {
namespace sophos {

// Forward declaration of some GRPC types

// Because Stub is a nested class, we need to use a trick to forward-declare it
// See https://stackoverflow.com/a/50619244
#ifndef SOPHOS_CLIENT_RUNNER_CPP
namespace Sophos {
class Stub;
}
#endif

class SearchRequestMessage;
class UpdateRequestMessage;

class SophosClientRunner
{
public:
    SophosClientRunner(const std::string& address, const std::string& path);
    ~SophosClientRunner();

    const SophosClient& client() const;

    std::list<uint64_t> search(
        const std::string&                   keyword,
        const std::function<void(uint64_t)>& receive_callback = nullptr) const;
    void update(const std::string& keyword, uint64_t index);
    void async_update(const std::string& keyword, uint64_t index);

    void start_update_session();
    void end_update_session();
    void update_in_session(const std::string& keyword, uint64_t index);

    void wait_updates_completion();

    bool load_inverted_index(const std::string& path);

    std::ostream& print_stats(std::ostream& out) const;

private:
    void update_completion_loop();

    bool send_setup() const;

    std::unique_ptr<sophos::Sophos::Stub> stub_;
    std::unique_ptr<SophosClient>         client_;

    struct
    {
        std::unique_ptr<grpc::ClientWriter<sophos::UpdateRequestMessage>>
                                               writer;
        std::unique_ptr<::grpc::ClientContext> context;
        ::google::protobuf::Empty              response;

        std::mutex mtx;
        bool       is_up;
    } bulk_update_state_;

    grpc::CompletionQueue update_cq_;

    std::atomic_size_t      update_launched_count_, update_completed_count_;
    std::thread*            update_completion_thread_;
    std::mutex              update_completion_mtx_;
    std::condition_variable update_completion_cv_;
    bool                    stop_update_completion_thread_;

    std::mutex update_mtx_;
};

SearchRequestMessage request_to_message(const SearchRequest& req);
UpdateRequestMessage request_to_message(const UpdateRequest& req);

} // namespace sophos
} // namespace sse
