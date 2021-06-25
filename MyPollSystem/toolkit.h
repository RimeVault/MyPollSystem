typedef struct resi{
    int id, nod, zc;
    float ic;
    char fn[20], ln[20];
    struct resi *next;
}resi;

int getSubstring(const char *pSrcString,const int index,char *pSubString,const int pSubStringSize);

int read_data(char *fname, resi *data);

int read_data_pipe(int fd, resi *data, int m);

void write_data(char *fname, resi *data, int n);

void write_data_pipe(int fd, resi *data, int n);

float get_feature(resi *data, int attr);

float bubble_sort(resi *data, int size, int order, int attri);

float select_sort(resi *data, int size, int order, int attri);

void destroy(resi *data);
