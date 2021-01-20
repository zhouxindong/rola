#ifndef ROLA_EASY_ACTOR_ERROR_HANDLE_HPP
#define  ROLA_EASY_ACTOR_ERROR_HANDLE_HPP

#include <string>
#include <functional>
#include <iostream>

namespace rola
{
	namespace actor
	{
        enum ErrorId
        {
            UVWriteFail = -2048,
            UVConnectFail,
            UVDisconnectFromServer,
            UVSigPipe,

            UndefinedError = -1024,
            NoFindActorName,
            ActorNameTooLong,
            NoFindActorAddr,
            ReDefineActorName,
            MessagePackNull,
            PackMessageError,
            NoFindRemoteFramework,

            RepeatedRemoteFrameworkID,
        };

		class Error_info
		{
		public:
            Error_info(ErrorId id, std::string info)
                : id_(id), info_(info)
            {}

            ErrorId error_id() const
            {
                return id_;
            }

            std::string& error_info()
            {
                return info_;
            }

            std::string const& error_info() const
            {
                return info_;
            }

        private:
            ErrorId id_;
            std::string info_;
		};

        class Error_handle
        {
        public:
            static Error_handle* Instance()
            {
                static Error_handle handle;
                return &handle;
            }

            void bind(std::function<void(Error_info)> f)
            {
                handle_ = f;
            }

            void error(Error_info info)
            {
                if (handle_)
                    handle_(info);
                else
                    std::cerr << "error id " << info.error_id() << ":" << info.error_info() << std::endl;
            }

        private:
            std::function<void(Error_info)> handle_;
            Error_handle()
            {}
        };
	} // namespace actor
} // namespace rola

#endif // !ROLA_EASY_ACTOR_ERROR_HANDLE_HPP
