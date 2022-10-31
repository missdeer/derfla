#include <AppKit/AppKit.h>

void HideDockIcon() {
    [NSApp setActivationPolicy: NSApplicationActivationPolicyProhibited];
}
