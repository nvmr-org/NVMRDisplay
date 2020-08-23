#include "nvmrdisplay.h"
#include "ui_nvmrdisplay.h"
#include "rpisourcebin.h"
#include "displaybin.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QRunnable>
#include <QLayout>
#include <QPushButton>

#include <log4cxx/logger.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.nvmrdisplay" );

class SetPlaying : public QRunnable
{
public:
    SetPlaying(GstElement * pipeline){
        this->pipeline_ = pipeline ? static_cast<GstElement *> (gst_object_ref (pipeline)) : NULL;
    }

    ~SetPlaying(){
        if (this->pipeline_)
          gst_object_unref (this->pipeline_);
    }

    void run (){
        LOG4CXX_DEBUG( logger, "DO THE RUN" );
        if (this->pipeline_)
          gst_element_set_state (this->pipeline_, GST_STATE_PLAYING);
    }

private:
  GstElement * pipeline_;
};

NVMRDisplay::NVMRDisplay(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NVMRDisplay),
    m_numberEntered( 0 ),
    m_currentCommand( CurrentCommand::NoCommand ),
    m_mainLayout( nullptr )
{
    ui->setupUi(this);


    ui->centralWidget->setLayout( &m_mainLayout );
    //playVideo( ui->quickWidget );
}

NVMRDisplay::~NVMRDisplay()
{
    delete ui;
}

void NVMRDisplay::keyPressEvent(QKeyEvent *event){
    if( m_currentCommand == CurrentCommand::NoCommand &&
            isKeySpecial( event ) ){
        LOG4CXX_TRACE( logger, "Got special key" );
        if( event->key() == Qt::Key_Asterisk ){
            m_currentCommand = CurrentCommand::ViewCamera;
        }else if( event->key() == Qt::Key_Plus ){
            m_currentCommand = CurrentCommand::AddCamera;
        }
    }else if( m_currentCommand != CurrentCommand::NoCommand &&
              isKeySpecial( event ) ){
        // This is not a key we are allowed to press at the moment!
        // TODO need to alert the user and reset?
        m_numberEntered = 0;
        return;
    }

    if( event->key() >= Qt::Key_0 &&
            event->key() <= Qt::Key_9 &&
            m_currentCommand != CurrentCommand::NoCommand ){
        m_numberEntered *= 10;
        m_numberEntered += (event->key() - Qt::Key_0);
    }

    if( event->key() == Qt::Key_Enter ){
        LOG4CXX_DEBUG( logger, "Do command " << m_currentCommand << " with number entered " << m_numberEntered );

        switch( m_currentCommand ){
        case CurrentCommand::ViewCamera:
        {
            // Show only the one camera
            for( QQuickWidget* wid: m_videos ){
                m_mainLayout.removeWidget( wid );
                wid->deleteLater();
            }
            m_videos.clear();

            QQuickWidget* displayWidget = new QQuickWidget();
            m_mainLayout.addWidget( displayWidget );
            m_videos.push_back( displayWidget );
            playVideo( displayWidget );
            break;
        }
        case CurrentCommand::AddCamera:
        {
            // Remove the first video
            if( m_videos.size() > 3 ){
                m_videos[ 0 ]->deleteLater();
                m_mainLayout.removeWidget( m_videos[ 0 ] );
                m_videos.pop_front();
            }

            // Remove all of the videos from the layout so we add them back in order.
//            for( QQuickWidget* wid: m_videos ){
//                //wid->setParent( this );
//                LOG4CXX_DEBUG( logger, "removing height: "
//                               << wid->size().height()
//                               << " width: "
//                               << wid->size().width() );
//                m_mainLayout.removeWidget( wid );

//            }

            QGridLayout* newLayout = new QGridLayout();
            QQuickWidget* displayWidget = new QQuickWidget();
            m_videos.push_back( displayWidget );
            m_mainLayout.addWidget( displayWidget );

//            int row = 0;
//            int col = 0;
//            for( QQuickWidget* wid : m_videos ){
//                m_mainLayout.removeWidget( wid );
////                m_mainLayout.addWidget( wid );
//                LOG4CXX_DEBUG( logger, "add at " << row << ":" << col );
//                newLayout->addWidget( wid, row, col );
//                col++;
//                if( col > 1 ){
//                    col = 0;
//                    row++;
//                }
//            }

            playVideo( displayWidget );

            break;
        }
        }

        m_currentCommand = CurrentCommand::NoCommand;
        m_numberEntered = 0;
    }
}

bool NVMRDisplay::isKeySpecial( QKeyEvent* evt ) const {
    switch( evt->key() ){
    case Qt::Key_Asterisk:
    case Qt::Key_Backslash:
    case Qt::Key_Plus:
    case Qt::Key_Period:
        return true;
    default:
        return false;
    }
}

static gboolean
my_bus_callback (GstBus * bus, GstMessage * message, gpointer data)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err;
      gchar *debug;


      gst_message_parse_error (message, &err, &debug);
      LOG4CXX_ERROR( logger, err->message );
      g_error_free (err);
      g_free (debug);

      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      //g_main_loop_quit (loop);
      break;
    default:
      /* unhandled message */
      break;
  }

  /* we want to be notified again the next time there is a message
   * on the bus, so returning TRUE (FALSE means we want to stop watching
   * for messages on the bus and our callback should not be called again)
   */
  return TRUE;
}

#if 1
void NVMRDisplay::playVideo(QQuickWidget *widget){
    widget->setSource(QUrl("qrc:/quickwidget/video.qml"));
    widget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    RPISourceBin* srcBin = new RPISourceBin();
    srcBin->setPort( 5248 );
    DisplayBin* dispBin = new DisplayBin();

    dispBin->linkToQQuickWidget( widget );

    GstElement *pipeline = gst_pipeline_new ("foopipeline");
    GstElement* source = srcBin->getBin();
//    GstElement* source = gst_element_factory_make( "videotestsrc", "src" );
    GstElement* sink = dispBin->bin();

    GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    guint bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, NULL);
    gst_object_unref (bus);

    gst_bin_add_many (GST_BIN (pipeline), source, sink, nullptr);
    LOG4CXX_ERROR( logger, "ABOUT TO LINK" );
    if( !gst_element_link( source, sink ) ){
        LOG4CXX_ERROR( logger, "src to sink link" );
    }

    widget->quickWindow()->scheduleRenderJob (new SetPlaying (pipeline),
          QQuickWindow::BeforeSynchronizingStage);
}

#else
void NVMRDisplay::playVideo(QQuickWidget *widget){
    widget->setSource(QUrl("qrc:/quickwidget/video.qml"));
    widget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    GstElement *pipeline = gst_pipeline_new ("foopipeline");
    GstElement *src = gst_element_factory_make ("videotestsrc", NULL);
    GstElement* convert = gst_element_factory_make( "autovideoconvert", NULL );

    GstElement* sinkQml = gst_element_factory_make ("qmlglsink", nullptr);
    GstElement* sinkBin = gst_element_factory_make("glsinkbin", nullptr);

    g_object_set (sinkBin, "sink", sinkQml, nullptr);

    gst_bin_add_many (GST_BIN (pipeline), src, convert, sinkBin, nullptr);
    gst_element_link_many (src, convert, sinkBin, nullptr);

    GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    guint bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, NULL);
    gst_object_unref (bus);

    if( pipeline == nullptr || src == nullptr || sinkQml == nullptr ){
        LOG4CXX_ERROR( logger, "something null" );
        return;
    }

    QQuickItem* quickRoot = widget->rootObject();
    g_object_set( sinkQml,
                  "widget", quickRoot->findChild<QQuickItem*>( "videoItem" ),
                  nullptr );

    LOG4CXX_DEBUG( logger, "schedule the render job.  ptr is " << quickRoot->findChild<QQuickItem*>( "videoItem" ) );
    widget->quickWindow()->scheduleRenderJob (new SetPlaying (pipeline),
          QQuickWindow::BeforeSynchronizingStage);
}
#endif
