#include "proc.h"

namespace v86 {
	void IProc::setMemory(IMemory* memory) {
		if (m_Memory != memory) {
			if (m_Memory) {
				m_Memory->drop();
			}

			if ((m_Memory = memory) != nullptr) {
				m_Memory->grab();
			}
		}
	}

	void IProc::setPort(IPort* port) {
		if (m_Ports != port) {
			if (m_Ports) {
				m_Ports->drop();
			}

			if ((m_Ports = port) != nullptr) {
				m_Ports->grab();
			}
		}
	}

	uint8_t IProc::inb(uint16_t port) {
		if (m_Ports) {
			uint8_t out;
			m_Ports->read(port, &out);
			return out;
		}

		return 0xff;
	}

	void IProc::outb(uint16_t port, uint8_t value) {
		if (m_Ports) {
			m_Ports->write(port, value);
		}
	}

	uint32_t IProc::read(uint32_t addr, void* buf, uint32_t size) {
		if (m_Memory) {
			return m_Memory->read(addr, buf, size);
		}

		return 0;
	}

	uint32_t IProc::write(uint32_t addr, const void* buf, uint32_t size) {
		if (m_Memory) {
			return m_Memory->write(addr, buf, size);
		}

		return 0;
	}
}