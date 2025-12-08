#include "Base/Sem.h"
#include "Base/New.h"

Sem* Sem::createNew(int val)
{
    //return new Sem(val);
    return New<Sem>::allocate(val);
}

Sem::Sem(int val)
{
    sem_init(&mSem, 0, val);
}

Sem::~Sem()
{
    sem_destroy(&mSem);
}

void Sem::post()
{
    sem_post(&mSem);
}

void Sem::wait()
{
    sem_wait(&mSem);
}
