#pragma once

namespace Orbit::Input::VirtualKeys
{
	// Keyboard codes no real keyboard sends. A command slot owns one; another mod's hotkey is bound to it in that
	// mod's MCM, and choosing the slot taps it - so a command costs no key of the real keyboard.
	bool IsVirtual(std::uint32_t a_code);

	// The first virtual code not in a_used; 0 when all are taken.
	std::uint32_t NextFree(const std::vector<std::uint32_t> &a_used);

	// Any thread: the key goes down on the next input frame and up on the one after.
	void Tap(std::uint32_t a_code);

	// Binding in another mod's MCM: while armed, the next press of the Orbit Menu key inside the journal (where the
	// MCM lives, waiting for a key) is sent as this code instead. 0 disarms.
	void Arm(std::uint32_t a_code);
	std::uint32_t Armed();

	// Input thread, from the dispatch hook.
	// True: the event was the armed press and has been taken (leave it out of the list).
	bool TakeArmedPress(const RE::InputEvent &a_event);
	// The synthetic events of this frame put before a_head; the new head.
	RE::InputEvent *Prepend(RE::InputEvent *a_head);
	// After the dispatch: the synthetic events freed.
	void Release();
}
