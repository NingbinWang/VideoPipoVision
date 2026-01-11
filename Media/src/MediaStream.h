#ifdef _MEDIA_STREAM_H_
#define _MEDIA_STREAM_H_

class MediaStream
{

    /* data */
public:
    static MediaStream* createNew(MEDIA_SHAREDATA_T* pShareData);
    MediaStream(MEDIA_SHAREDATA_T* pShareData);
    MediaStream();
   
private:
    MEDIA_SHAREDATA_T* mpShareData;
};







#endif
