#ifndef CAN_HPP
#define CAN_HPP

#include <ICommunication.hpp>
#include <stdint.h>

namespace Driver
{

class Can : public ICommunication
{
public:
	Can(uint8_t ownId);
	virtual ~Can() = default;
	uint8_t Init();
	uint8_t Init(uint8_t filterId);
	bool SendDataFrame(uint16_t id, uint8_t* pData, uint8_t len);
	bool SendRemoteFrame(uint16_t id);

private:
	uint8_t ownId;
};

} // namespace Driver

#endif // CAN_HPP
