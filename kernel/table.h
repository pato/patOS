#ifndef _TABLE_H_
#define _TABLE_H_

class Table : public virtual Resource {
    long n;
    ResourcePtr *array;
    Mutex mutex;
public:
    Table(long n);
    virtual ~Table();

    long open(Resource* p);
    long close(long i);
    void closeAll();
    Resource* get(long id, ResourceType type);
    Table* forkMe();
};

#endif
