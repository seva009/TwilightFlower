#pragma once

enum class commands
{
	VERSION = 0, //version
	DEC_SEC_KEY, //decrypt secondary key
	DEC_PMR_KEY, //decrypt primary master key
	RGN_SEC_KEY, //regenerate secondary key
	RGN_PMR_KEY, //regenerate primary master key
	ENC_SEC_KEY, //encrypt secondary key
	ENC_PMR_KEY, //encrypt primary master key
	SWP_USR_KEY, //swap user key
	GET_PMR_KEY, //get primary master key
	HALT         //halt
};