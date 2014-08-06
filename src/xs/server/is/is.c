
typedef struct bs_t
{
    char* name;
    char* address;
    char* cpu;
    char* memory;
    char* ip;
    unsigned short port;

    int userCount;
    int userCountMax;

} bs_t;

void server_init()
{}
