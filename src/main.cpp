/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#include <QtWidgets>

#include "mainwindow.h"
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <turntable.h>

#include <turntable.h>
#include <drummachine.h>

#include <utfilemgr.h>

class QSettings;
//class Turntable;
//class DrumMachine;


#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
// If OpenGL is not used, we're building for Symbian^1. If that is the case,
// for performance reasons let's also drop the accelerometer feature.
#ifndef QT_NO_OPENGL
#include <QSysInfo>
#include <QtSensors/QAccelerometer>
#include "accelerometerfilter.h"

#endif
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);



    QApplication app(argc, argv);
    app.setOrganizationName("turntable.lduboeuf");
    app.setApplicationName("turntable");

    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);


    // Lock orientation in Symbian
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE(
                if (appUi) {
                    appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
                }
                );
#endif

    //    MainWindow mainWindow;

    //#if defined(Q_WS_MAEMO_6) || defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    //    mainWindow.setGeometry(QApplication::desktop()->screenGeometry());
    //    mainWindow.showFullScreen();
    //#else
    //    mainWindow.setGeometry(QRect(100, 100, 640, 360));
    //    mainWindow.show();
    //#endif



    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();


#if defined(Q_WS_MAEMO_5) || defined(QT_NO_OPENGL)
    // Set UI to low performance mode for Maemo5 and Symbian^1. This mainly
    // disables antialiasing on some performance costly elements.
    context->setContextProperty("lowPerf", QVariant(true));
#else
    context->setContextProperty("lowPerf", QVariant(false));
#endif

    QString sampleFolder;
#ifdef Q_OS_UBUNTU_TOUCH
    sampleFolder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    UTFileMgr fileManager(sampleFolder);
    engine.rootContext()->setContextProperty("UBUNTU_TOUCH", true);
    engine.rootContext()->setContextProperty("utFileManager", &fileManager);
    context->setContextProperty("exitButtonVisible", QVariant(true));
#else
    sampleFolder = QStandardPaths::standardLocations(QStandardPaths::MusicLocation)[0];
#endif

    context->setContextProperty("sampleFolder", sampleFolder);
    //QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + "samples"
    //qDebug() << QDir::currentPath();


#if defined Q_WS_MAEMO_6 && !defined Q_OS_UBUNTU_TOUCH
    // Hide the exit button in Harmattan
    context->setContextProperty("exitButtonVisible", QVariant(false));
#else
    context->setContextProperty("exitButtonVisible", QVariant(true));
#endif

    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *rootObject = dynamic_cast<QObject*>(engine.rootObjects()[0]);

    // Create Qt settings object to load / store app settings
    QSettings m_settings;
    Turntable *m_turntable = new Turntable(&m_settings, rootObject);
    DrumMachine *m_drumMachine = new DrumMachine(&m_settings, rootObject);

    m_turntable->addAudioSource(m_drumMachine);


    // Find out the interesting Qt objects of the QML elements
    //QObject *turntableQML = rootObject->findChild<QObject*>("turntable");
    QObject *turntableQML = dynamic_cast<QObject*>(rootObject);
    if (turntableQML){
        qDebug() << "ok found turntable";
    }
    QObject *sampleSelectorQML =
            rootObject->findChild<QObject*>("sampleSelector");
    QObject *drumMachineQML =
            rootObject->findChild<QObject*>("drumMachine");

    // If there are errors in QML code and the elements does not exist,
    // they won't be found Qt side either, check existance of the elements.
    if (!turntableQML || !sampleSelectorQML || !drumMachineQML) {
        QMessageBox::warning(NULL, "Warning",
                             "Failed to resolve QML elements in main.cpp");
        return -1;
    }

    // Turntable connections
    QObject::connect(turntableQML, SIGNAL(start()),
                     m_turntable, SLOT(start()));
    QObject::connect(turntableQML, SIGNAL(stop()),
                     m_turntable, SLOT(stop()));
    QObject::connect(turntableQML, SIGNAL(diskAimSpeed(QVariant)),
                     m_turntable, SLOT(setDiscAimSpeed(QVariant)));
    QObject::connect(turntableQML, SIGNAL(diskSpeed(QVariant)),
                     m_turntable, SLOT(setDiscSpeed(QVariant)));
    QObject::connect(turntableQML, SIGNAL(cutOff(QVariant)),
                     m_turntable, SLOT(setCutOff(QVariant)));
    QObject::connect(turntableQML, SIGNAL(resonance(QVariant)),
                     m_turntable, SLOT(setResonance(QVariant)));
    QObject::connect(turntableQML, SIGNAL(seekToPosition(QVariant)),
                     m_turntable, SLOT(seekToPosition(QVariant)));
    QObject::connect(m_turntable, SIGNAL(audioPosition(QVariant)),
                     turntableQML, SLOT(audioPosition(QVariant)));
    QObject::connect(m_turntable, SIGNAL(powerOff()),
                     turntableQML, SLOT(powerOff()));

    // SampleSelector connections
    QObject::connect(sampleSelectorQML, SIGNAL(sampleSelected(QVariant)),
                     m_turntable, SLOT(setSample(QVariant)));
    QObject::connect(sampleSelectorQML, SIGNAL(defaultSample()),
                     m_turntable, SLOT(openDefaultSample()));
    QObject::connect(m_turntable, SIGNAL(sampleOpened(QVariant)),
                     sampleSelectorQML, SLOT(setCurrentSample(QVariant)));
    QObject::connect(m_turntable, SIGNAL(error(QVariant, QVariant)),
                     sampleSelectorQML, SLOT(showError(QVariant, QVariant)));

    // DrumMachine connections
    QObject::connect(drumMachineQML, SIGNAL(startBeat()),
                     m_drumMachine, SLOT(startBeat()));
    QObject::connect(drumMachineQML, SIGNAL(stopBeat()),
                     m_drumMachine, SLOT(stopBeat()));
    QObject::connect(drumMachineQML, SIGNAL(setBeat(QVariant)),
                     m_drumMachine, SLOT(setBeat(QVariant)));
    QObject::connect(drumMachineQML,
                     SIGNAL(drumButtonToggled(QVariant, QVariant, QVariant)),
                     m_drumMachine,
                     SLOT(drumButtonToggled(QVariant, QVariant, QVariant)));
    QObject::connect(drumMachineQML, SIGNAL(drumMachineSpeed(QVariant)),
                     m_drumMachine, SLOT(setBeatSpeed(QVariant)));
    QObject::connect(m_drumMachine,
                     SIGNAL(drumButtonState(QVariant, QVariant, QVariant)),
                     drumMachineQML,
                     SLOT(setDrumButton(QVariant, QVariant, QVariant)));
    QObject::connect(m_drumMachine, SIGNAL(tickChanged(QVariant)),
                     drumMachineQML, SLOT(highlightTick(QVariant)));

    // Framework connections
    //QObject::connect(engine, SIGNAL(quit()), qApp, SLOT(quit()));

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#ifndef QT_NO_OPENGL
    // Create Qt accelerometer objects
    QAccelerometer *m_accelerometer = new QAccelerometer(rootObject);
    AccelerometerFilter *m_accelerometerFilter = new AccelerometerFilter;
    // Does not take the ownership of the filter
    m_accelerometer->addFilter(m_accelerometerFilter);

    m_accelerometer->setDataRate(50);

    // Create Qt objects for accessing profile information
    //m_deviceInfo = new QSysInfo;
    //TODO    m_turntable->profile(m_deviceInfo->currentProfile());

    QObject::connect(m_accelerometerFilter, SIGNAL(rotationChanged(QVariant)),
                     turntableQML, SLOT(inclination(QVariant)));
    //   TODO connect(m_deviceInfo,
    //            SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
    //            m_turntable,
    //            SLOT(profile(QSystemDeviceInfo::Profile)));

    // Begin the measuring of the accelerometer sensor
    m_accelerometer->start();
#endif
#endif

    m_turntable->openLastSample();
    m_drumMachine->setBeat(0);

    return app.exec();
}

