#ifndef __V86_CPU_PROCESSOR_H__
#define __V86_CPU_PROCESSOR_H__
#include "reg.h"
#include "state.h"

#include "../dev/port.h"
#include "../dev/memory.h"
#include <string.h>

namespace v86 {
	class IProc {
	private:
		state_t m_State;
		IMemory* m_Memory;
		IPort* m_Ports;

	public:
		IProc() : m_Memory(nullptr), m_Ports(nullptr) {
			memset(&m_State, 0, sizeof(m_State));
		}

		virtual ~IProc() { }


	public:
		inline state_t* getState() const {
			return const_cast<state_t*>(&m_State);
		}

		inline IMemory* getMemory() const { return m_Memory; }
		inline IPort* getPort() const { return m_Ports; }

		/* simple definition macros. */
#define USE_STATE(proc, name)	v86::state_t*	name = (proc)->getState()
#define USE_FETCH_STATE(proc, name) \
								v86::fetch_t*	name = &((proc)->getState()->fetch)
#define USE_MEMORY(proc, name)	v86::IMemory*	name = (proc)->getMemory()
#define USE_PORT(proc, name)	v86::IPort*		name = (proc)->getPort()

		/* set the memory instance. */
		void setMemory(IMemory* memory);

		/* set the IO port instance. */
		void setPort(IPort* port);

	public:
		/* execute single step. */
		virtual void exec() = 0;

	public:
		/* io port in, byte. */
		virtual uint8_t inb(uint16_t port);

		/* io port out, byte. */
		virtual void outb(uint16_t port, uint8_t value);

		/* read bytes from the memory. */
		virtual uint32_t read(uint32_t addr, void* buf, uint32_t size);

		/* write bytes into the memory. */
		virtual uint32_t write(uint32_t addr, const void* buf, uint32_t size);

	public:
		/* fetch a code byte. */
		virtual uint8_t fetch() = 0;

		/* push bytes to the stack. */
		virtual void push(const void* buf, uint32_t size) = 0;

		/* pop bytes from the stack. */
		virtual void pop(void* buf, uint32_t size) = 0;
	};
}

#endif // __V86_CPU_PROCESSOR_H__
