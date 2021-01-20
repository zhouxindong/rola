#ifndef ROLA_UTILS_NTP_CLIENT_HPP
#define ROLA_UTILS_NTP_CLIENT_HPP

#include <easy_socket/udp_socket.h>

#include <stdint.h>
#include <chrono>

#include "byte_order.hpp"
#include <base/any_assert.hpp>

#include <easy_signal/signal_hub.hpp>

namespace rola
{
#pragma region ntp timestamp

	// Number of milliseconds passed since epoch on Jan 1 1900.
	constexpr static int64_t january_1_1900 = -2208988800000ll;

#pragma pack(push, 1)

	// 64-bit NTP timestamp
	struct Ntp_timestamp
	{
		// Number of seconds passed since Jan 1 1900, in big-endian format.
		uint32_t seconds;

		// Fractional time part, in <tt>1/0xFFFFFFFF</tt>s of a second.
		uint32_t fraction;

		static Ntp_timestamp from_timepoint(std::chrono::system_clock::time_point const& tp)
		{
			// Convert given time to number of milliseconds passed since Jan 1 1900.
			int64_t ntp_ms = std::chrono::duration_cast<std::chrono::milliseconds>
				(tp.time_since_epoch()).count() - january_1_1900;

			// Note that NTP epoch information is lost here. Assume 1900-2036 NTP epoch.
			uint32_t seconds = static_cast<uint32_t>(ntp_ms / 1000);
			uint32_t fraction = static_cast<uint32_t>(0x100000000ll * (ntp_ms % 1000) / 1000);

			Ntp_timestamp result;
			result.seconds = rola::to_big_endian(seconds);
			result.fraction = rola::to_big_endian(fraction);
			return result;
		}

		static std::chrono::system_clock::time_point to_timepoint(Ntp_timestamp const& ntp_time)
		{
			uint32_t seconds = rola::from_big_endian(ntp_time.seconds);
			uint32_t fraction = rola::from_big_endian(ntp_time.fraction);

			int64_t ntp_ms = seconds * 1000ll + fraction * 1000ll / 0x100000000ll;
			return std::chrono::system_clock::time_point(
				std::chrono::milliseconds(ntp_ms + january_1_1900));
		}
	};

#pragma pack(pop)

#pragma endregion

#pragma region ntp protocol

	/**
 * Leap indicators.
 */
	enum NtpLeapIndicator {
		NoWarning = 0,             /**< No warning. */
		LastMinute61Warning = 1,   /**< Last minute has 61 seconds. */
		LastMinute59Warning = 2,   /**< Last minute has 59 seconds. */
		UnsynchronizedWarning = 3, /**< Alarm condition (clock not synchronized). */
	};

	/**
	 * Modes.
	 */
	enum NtpMode {
		ReservedMode = 0,         /**< Reserved. */
		SymmetricActiveMode = 1,  /**< Symmetric active. */
		SymmetricPassiveMode = 2, /**< Symmetric passive. */
		ClientMode = 3,           /**< Client. */
		ServerMode = 4,           /**< Server. */
		BroadcastMode = 5,        /**< Broadcast. */
		ControlMode = 6,          /**< NTP control message. */
		PrivateMode = 7,          /**< Reserved for private use. */
	};

	/**
	 * Strata.
	 */
	enum NtpStratum {
		UnspecifiedStratum = 0,     /**< Unspecified or unavailable. */
		PrimaryStratum = 1,         /**< Primary reference (e.g. radio-clock). */
		SecondaryStratumFirst = 2,  /**< Secondary reference (via NTP or SNTP). */
		SecondaryStratumLast = 15,
		UnsynchronizedStratum = 16, /**< Unsynchronized. */
		/* 17-255 are reserved. */
	};
#pragma endregion

#pragma region ntp packet

	/*
	* NtpPacket flags (first byte of the packet).
	*/
	struct Ntp_packet_flags
	{
		/** Mode. */
		unsigned char mode : 3;

		/** Version number of the protocol (3 or 4). */
		unsigned char version_number : 3;

		/** Warns of an impending leap second to be inserted/deleted in the last minute
		 * of the current day. */
		unsigned char leap_indicator : 2;
	};

#pragma pack(push, 1) // push old align setting, and set alignment to 1 byte

	/*
	* Mandatory part of an NTP packet
	*/
	struct Ntp_packet {
		/** Flags. */
		Ntp_packet_flags flags;

		/** Stratum of the clock. */
		uint8_t stratum;

		/** Maximum interval between successive messages, in log2 seconds. Note that the value is signed. */
		int8_t poll;

		/** Precision of the clock, in log2 seconds. Note that the value is signed. */
		int8_t precision;

		/** Round trip time to the primary reference source, in NTP short format. */
		int32_t rootDelay;

		/** Nominal error relative to the primary reference source. */
		int32_t rootDispersion;

		/** Reference identifier (either a 4 character string or an IP address). */
		int8_t referenceID[4];

		/** The time at which the clock was last set or corrected. */
		Ntp_timestamp referenceTimestamp;

		/** The time at which the request departed the client for the server. */
		Ntp_timestamp originateTimestamp;

		/** The time at which the request arrived at the server. */
		Ntp_timestamp receiveTimestamp;

		/** The time at which the reply departed the server for client. */
		Ntp_timestamp transmitTimestamp;
	}; // struct Ntp_packet

	/*
	* Optional part of an NTP packet.
	*/
	struct Ntp_authenticator
	{
		uint32_t keyId;
		uint8_t messageDigest[16];
	};

	/*
	* Full NTP packet.
	*/
	struct Ntp_full_packet
	{
		Ntp_packet basic;
		Ntp_authenticator auth;
	};

#pragma pack(pop)

#pragma endregion

#pragma region ntp reply

	class Ntp_reply_private
	{
	public:
		Ntp_full_packet packet;
		std::chrono::system_clock::time_point destinationTime;
	};

	class Ntp_reply
	{
	public:
		/**
		 * Default constructor. Constructs a null ntp reply.
		 */
		Ntp_reply()
			: d(new Ntp_reply_private())
		{}

		Ntp_reply(const Ntp_reply& other)
			: d(other.d)
		{}

		~Ntp_reply()
		{}

		Ntp_reply& operator=(const Ntp_reply& other)
		{
			d = other.d;
			return *this;
		}

		/**
		 * @returns                        Leap indicator.
		 */
		NtpLeapIndicator leapIndicator() const
		{
			return static_cast<NtpLeapIndicator>(
				d->packet.basic.flags.leap_indicator);
		}

		/**
		 * @returns                        NTP version number.
		 */
		uint8_t versionNumber() const
		{
			return d->packet.basic.flags.version_number;
		}

		/**
		 * @returns                        Ntp mode.
		 */
		NtpMode mode() const
		{
			return static_cast<NtpMode>(d->packet.basic.flags.mode);
		}

		/**
		 * @returns                        Stratum.
		 */
		uint8_t stratum() const
		{
			return d->packet.basic.stratum;
		}

		/**
		 * @returns                        Maximum interval between successive messages, in seconds.
		 */
		double pollInterval() const
		{
			return std::pow(static_cast<double>(2),
				static_cast<double>(d->packet.basic.poll));
		}

		/**
		 * @returns                        Clock precision, in seconds.
		 */
		double precision() const
		{
			return std::pow(static_cast<double>(2),
				static_cast<double>(d->packet.basic.precision));
		}

		/**
		 * @returns                        Time when the system clock was last set or corrected.
		 */
		std::chrono::system_clock::time_point referenceTime() const
		{
			return Ntp_timestamp::to_timepoint(d->packet.basic.referenceTimestamp);
		}

		/**
		 * @returns                        Time at the client when the request departed for the server.
		 */
		std::chrono::system_clock::time_point originTime() const
		{
			return Ntp_timestamp::to_timepoint(d->packet.basic.originateTimestamp);
		}

		/**
		 * @returns                        Time at the server when the request arrived from the client.
		 */
		std::chrono::system_clock::time_point receiveTime() const
		{
			return Ntp_timestamp::to_timepoint(d->packet.basic.receiveTimestamp);
		}

		/**
		 * @returns                        Time at the server when the response left for the client.
		 */
		std::chrono::system_clock::time_point transmitTime() const
		{
			return Ntp_timestamp::to_timepoint(d->packet.basic.transmitTimestamp);
		}

		/**
		 * @returns                        Time at the client when the reply arrived from the server.
		 */
		std::chrono::system_clock::time_point destinationTime() const
		{
			return d->destinationTime;
		}

		/**
		 * @returns                        Round trip delay, in milliseconds.
		 */
		int64_t roundTripDelay() const
		{
			auto diff = destinationTime() - originTime();
			auto diff2 = transmitTime() - receiveTime();
			auto delay = diff - diff2;
			return std::chrono::duration_cast<std::chrono::milliseconds>(delay).count();
		}

		/**
		 * @returns                        Local clock offset, in milliseconds.
		 */
		int64_t localClockOffset() const
		{
			auto diff = receiveTime() - originTime();
			auto diff2 = transmitTime() - destinationTime();
			auto offset = (diff + diff2) / 2;
			return std::chrono::duration_cast<std::chrono::milliseconds>(offset).count();
		}

		/**
		 * @returns                        Whether this ntp reply is null.
		 */
		bool isNull() const
		{
			return d->destinationTime == std::chrono::system_clock::time_point();
		}

	protected:
		friend class Ntp_std_client; /* Calls protected constructor. */

		Ntp_reply(Ntp_reply_private* dd)
			: d(dd)
		{
			ASSERT_ANY(dd != nullptr);
		}

	private:
		std::shared_ptr<Ntp_reply_private> d;
	};

	inline std::ostream& operator<<(std::ostream& out, Ntp_reply const& reply)
	{
		out << "Leap Indicator: " << reply.leapIndicator() << std::endl;
		out << "Version Number: " << static_cast<int>(reply.versionNumber()) << std::endl;
		out << "Mode: " << reply.mode() << std::endl;
		out << "Stratum: " << static_cast<int>(reply.stratum()) << std::endl;
		out << "Poll Interval: " << reply.pollInterval() << std::endl;
		out << "Precision: " << reply.precision() << std::endl;
		out << "Reference Time: " << reply.referenceTime().time_since_epoch().count() << std::endl;
		out << "Origin Time: " << reply.originTime().time_since_epoch().count() << std::endl;
		out << "Receive Time: " << reply.receiveTime().time_since_epoch().count() << std::endl;
		out << "Transmit Time: " << reply.transmitTime().time_since_epoch().count() << std::endl;
		out << "Destination Time: " << reply.destinationTime().time_since_epoch().count() << std::endl;
		out << "Round Trip Delay: " << reply.roundTripDelay() << std::endl;
		out << "Local Clock Offset: " << reply.localClockOffset() << std::endl;
		return out;
	}

#pragma endregion

#pragma region ntp std client

	class Ntp_std_client
	{
		static constexpr int SendIntervalMs = 200;

	public:
		Ntp_std_client()
		{
			init_(rola::inet_address(0));
		}

		Ntp_std_client(in_port_t port)
		{
			init_(rola::inet_address(port));
		}

		Ntp_std_client(std::string const& ip, in_port_t port)
		{
			init_(rola::inet_address(ip, port));
		}

		~Ntp_std_client()
		{}

	public:
		static rola::Easy_signal<void(rola::Ntp_reply const&)>* get_signal(std::string const& sig_name)
		{
			static rola::Signal_hub<void(rola::Ntp_reply const&)> sig_hub;
			return sig_hub.get_signal(sig_name);
		}

		static std::string ntp_reply_signal()
		{
			return "NtpReplySignal";
		}

	public:

		bool send_request(std::string const& ip, in_port_t port,
			std::function<std::chrono::system_clock::time_point()> get_now = std::chrono::system_clock::now)
		{
			rola::Ntp_packet packet;
			::memset(&packet, 0, sizeof(packet));
			packet.flags.mode = ClientMode;
			packet.flags.version_number = 4;
			packet.transmitTimestamp = rola::Ntp_timestamp::from_timepoint(get_now());

			rola::inet_address ntp_svr(ip, port);
			auto slen = udp_.send_to(reinterpret_cast<const char*>(&packet), sizeof(packet), ntp_svr);
			if (slen == -1)
				return false;

			int count = 0;
			do
			{
				rola::Ntp_full_packet packet;
				::memset(&packet, 0, sizeof(packet));

				auto len = udp_.recv(reinterpret_cast<char*>(&packet), sizeof(packet));
				if (len > 0)
				{
					std::chrono::system_clock::time_point now = get_now();
					rola::Ntp_reply_private* reply_p = new Ntp_reply_private();
					reply_p->packet = packet;
					reply_p->destinationTime = now;
					rola::Ntp_reply reply(reply_p);

					get_signal(ntp_reply_signal())->emit_(reply);
					return true;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(SendIntervalMs));
			} while (++count < 3);

			return false;
		}

	private:
		void init_(rola::inet_address const& addr)
		{
			udp_.bind(addr);
			udp_.set_non_blocking(true);
		}

	private:
		rola::socket_initializer socket_initer_;
		rola::udp_socket udp_;
	};

#pragma endregion

} // namespace rola

#endif // !ROLA_UTILS_NTP_CLIENT_HPP
