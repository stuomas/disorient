#ifndef AUDIOENDPOINTCONTROLLER_H
#define AUDIOENDPOINTCONTROLLER_H

#include <PolicyConfig.h>
#include <QObject>
#include <QVector>

#define DEVICE_OUTPUT_FORMAT "Audio Device %d: %ws"

typedef struct TGlobalState
{
    HRESULT hr;
    int option;
    IMMDeviceEnumerator *pEnum;
    IMMDeviceCollection *pDevices;
    LPWSTR strDefaultDeviceID;
    IMMDevice *pCurrentDevice;
    LPCWSTR pDeviceFormatStr;
    int deviceStateFilter;
} TGlobalState;

class AudioEndpointController : public QObject
{
    Q_OBJECT

public:
    AudioEndpointController();
    ~AudioEndpointController();
    HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID);
    HRESULT printDeviceInfo(IMMDevice* pDevice, int index);
    void createDeviceEnumerator(TGlobalState* state);
    void prepareDeviceEnumerator(TGlobalState* state);
    void enumerateOutputDevices(TGlobalState* state);
    std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key);
    QVector<QString> getAllAudioDevices();
    void refreshList();

public slots:
    void onChangeRequest(QString device);

private:
    TGlobalState *m_state = new TGlobalState;
    QString m_requestedDevice;
    QVector<QString> allAudioDevices;
};

#endif // AUDIOENDPOINTCONTROLLER_H
