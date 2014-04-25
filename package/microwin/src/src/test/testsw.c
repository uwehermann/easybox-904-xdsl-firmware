#include "include/device.h"
#include "include/mwtypes.h"
int a, c;

main()
{
	switch (c) {
	case MWPF_TRUECOLOR8888:
	case MWPF_TRUECOLOR0888:
	case MWPF_TRUECOLOR888:
		a = 1;
		break;
	case MWPF_TRUECOLORABGR:
		a = 2;
	case MWPF_TRUECOLOR565:
		a = 3;
		break;
	case MWPF_TRUECOLOR555:
		a = 4;
		break;
	case MWPF_TRUECOLOR332:
		a = 5;
		break;
	case MWPF_PALETTE:
	//default:
		a = 6;
		break;
	case MWPF_RGB:		// 0
		a = 7;
		break;
	}
}
