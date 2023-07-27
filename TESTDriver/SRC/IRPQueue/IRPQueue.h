#pragma once

void KernelCustomEventInit(BOOLEAN initState, KEVENT* EventData);
void set(KEVENT* EventData);
void reset(KEVENT* EventData);
void wait(KEVENT* EventData);
BOOL readState(KEVENT* EventData);
