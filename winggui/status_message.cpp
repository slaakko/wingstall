// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <winggui/status_message.hpp>

namespace wingstall { namespace winggui {

StatusMessage::StatusMessage(StatusMessageKind kind_) : kind(kind_)
{
}

StatusMessage::~StatusMessage()
{
}

StatusChangedMessage::StatusChangedMessage(const Status& status_, const std::string& statusStr_, const std::string& errorMessage_) : 
    StatusMessage(StatusMessageKind::statusChanged), status(status_), statusStr(statusStr_), errorMessage(errorMessage_)
{
}

ComponentChangedMessage::ComponentChangedMessage(const std::string& component_) : StatusMessage(StatusMessageKind::componentChanged), component(component_)
{
}

FileChangedMessage::FileChangedMessage(const std::string& file_) : StatusMessage(StatusMessageKind::fileChanged), file(file_)
{
}

StreamPositionChangedMessage::StreamPositionChangedMessage(int64_t position_) : StatusMessage(StatusMessageKind::streamPositionChanged), position(position_)
{
}

FileIndexChangedMessage::FileIndexChangedMessage(int fileIndex_) : StatusMessage(StatusMessageKind::fileIndexChanged), fileIndex(fileIndex_)
{
}

LogErrorMessage::LogErrorMessage(const std::string& error_) : StatusMessage(StatusMessageKind::logError), error(error_)
{
}

StatusMessageQueue::StatusMessageQueue()
{
}

void StatusMessageQueue::Put(StatusMessage* message)
{
    std::lock_guard<std::mutex> lock(mtx);
    q.push(std::unique_ptr<StatusMessage>(message));
}

std::unique_ptr<StatusMessage> StatusMessageQueue::Get()
{
    std::lock_guard<std::mutex> lock(mtx);
    if (q.empty())
    {
        throw std::runtime_error("status message queue empty");
    }
    std::unique_ptr<StatusMessage> message = std::move(q.front());
    q.pop();
    return message;
}

} } // namespace wingstall::wingui
