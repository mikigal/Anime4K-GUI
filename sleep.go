package main

import (
	"fmt"
	"syscall"
)

var (
	kernel32                    = syscall.NewLazyDLL("kernel32.dll")
	procSetThreadExecutionState = kernel32.NewProc("SetThreadExecutionState")
)

const (
	ES_CONTINUOUS       = 0x80000000
	ES_SYSTEM_REQUIRED  = 0x00000001
	ES_DISPLAY_REQUIRED = 0x00000002
)

func preventSleep() {
	ret, _, err := procSetThreadExecutionState.Call(
		uintptr(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED),
	)

	if ret == 0 {
		logMessage(fmt.Sprintf("Failed to block sleep/hibernation: %v\n", err), false)
	}
}
