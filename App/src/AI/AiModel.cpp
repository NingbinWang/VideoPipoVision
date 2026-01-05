#include "Media.h"
#include "AiModel.h"
#include "Common.h"
#include "autoconf.h"
#include "AiParse.h"

int AiModelInit()
{
	unsigned char *model;
	int model_size = 0;
	model = AiReadFileData(MODEL_PATH,&model_size);
	MediaAi_Init(model,model_size,LABEL_NALE_TXT_PATH);
	return 0;
}


