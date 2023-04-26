#include "globals.hpp"
#include "src/Window.hpp"
#include <src/plugins/PluginAPI.hpp>
#include <src/Compositor.hpp>

inline CFunctionHook* g_LastSelectionHook = nullptr;

void hook_update_decos(void* thisptr, CWindow* window) {
	bool explicitly_selected = g_Hy3Layout->shouldRenderSelected(window);
	Debug::log(LOG, "update decos for %p - selected: %d", window, explicitly_selected);

	auto* lastWindow = g_pCompositor->m_pLastWindow;
	if (explicitly_selected) {
		g_pCompositor->m_pLastWindow = window;
	}

	((void (*)(void*, CWindow*)) g_LastSelectionHook->m_pOriginal)(thisptr, window);

	if (explicitly_selected) {
		g_pCompositor->m_pLastWindow = lastWindow;
	}
}

void setup_selection_hook() {
	if (g_LastSelectionHook == nullptr) {
		static const auto decoUpdateCandidates = HyprlandAPI::findFunctionsByName(PHANDLE, "updateWindowAnimatedDecorationValues");

		if (decoUpdateCandidates.size() != 1) {
			Debug::log(ERR, "Expected one matching function to hook for \"updateWindowAnimatedDecorationValues\", found %d", decoUpdateCandidates.size());
			return;
		}

		g_LastSelectionHook = HyprlandAPI::createFunctionHook(PHANDLE, decoUpdateCandidates[0].address, (void *)&hook_update_decos);
	}

	g_LastSelectionHook->hook();
}

void disable_selection_hook() {
	if (g_LastSelectionHook != nullptr) {
		g_LastSelectionHook->unhook();
	}
}
