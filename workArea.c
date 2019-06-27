typedef struct {
    int x, y;
    int width, height;
} WorkAreaData;

WorkAreaData* getWorkAreaData() {
    WorkAreaData **workAreaData = (WorkAreaData**)malloc(sizeof(WorkAreaData*));
    *workAreaData = (WorkAreaData*)malloc(sizeof(WorkAreaData));

    RECT workAreaSystemData;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaSystemData, 0);
    (*workAreaData)->x      = workAreaSystemData.left;
    (*workAreaData)->y      = workAreaSystemData.top;
    (*workAreaData)->width  = workAreaSystemData.right - workAreaSystemData.left;
    (*workAreaData)->height = workAreaSystemData.bottom - workAreaSystemData.top;

    return *workAreaData;
}