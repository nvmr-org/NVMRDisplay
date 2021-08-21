#include <log4cxx/logger.h>
#include <QRunnable>

#include "rpivideodisplay.h"
#include "displaybin.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.RPIVideoDisplay" );

class SetPlaying : public QRunnable
{
public:
    SetPlaying(GstElement * pipeline, RPIVideoSender* vidSend, GstState state){
        this->pipeline_ = pipeline ? static_cast<GstElement *> (gst_object_ref (pipeline)) : NULL;
        m_vidSend = vidSend;
        m_state = state;
    }

    ~SetPlaying(){
        if (this->pipeline_)
          gst_object_unref (this->pipeline_);
    }

    void run (){
        LOG4CXX_DEBUG( logger, "Setting pipeline to " << m_state );
        if (this->pipeline_){
          gst_element_set_state (this->pipeline_, m_state);
        }

        if( m_vidSend && m_state == GST_STATE_PLAYING ){
            RPISourceBin* bin = static_cast<RPISourceBin*>( m_vidSend->getVideoSource() );
            m_vidSend->startStreamingVideo( bin->port() );
        }
    }

private:
  GstElement * pipeline_;
  RPIVideoSender* m_vidSend;
  GstState m_state;
};

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

RPIVideoDisplay::RPIVideoDisplay(RPIVideoSender* vidsend,
                                 QObject *parent) :
    QObject(parent),
    m_videoSender( vidsend )
{
    m_videoWidget = new QQuickWidget();
    m_videoWidget->setSource(QUrl("qrc:/quickwidget/video.qml"));
    m_videoWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    m_displayBin = new DisplayBin();

    m_displayBin->linkToQQuickWidget( m_videoWidget );

    m_pipeline = gst_pipeline_new ("foopipeline");
    GstElement* source = m_videoSender->getVideoSource()->getBin();
//    GstElement* source = gst_element_factory_make( "videotestsrc", "src" );
    GstElement* sink = m_displayBin->bin();

    GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (m_pipeline));
    guint bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, NULL);
    gst_object_unref (bus);

    gst_bin_add_many (GST_BIN (m_pipeline), source, sink, nullptr);
    if( !gst_element_link( source, sink ) ){
        LOG4CXX_ERROR( logger, "Unable to link source to sink" );
    }
}

RPIVideoDisplay::~RPIVideoDisplay(){
    stop();
    m_videoSender->stopStreamingVideo();
    delete m_videoWidget;
    delete m_displayBin;
}

void RPIVideoDisplay::play(){
    LOG4CXX_DEBUG( logger, "Playing video " << videoId() );
    m_displayBin->linkToQQuickWidget( m_videoWidget );
    m_videoWidget->quickWindow()->scheduleRenderJob (new SetPlaying (m_pipeline, m_videoSender, GST_STATE_PLAYING),
          QQuickWindow::BeforeSynchronizingStage);
    m_videoWidget->repaint();
}

void RPIVideoDisplay::stop(){
    LOG4CXX_DEBUG( logger, "Stopping video " << videoId() );
    m_videoWidget->quickWindow()->scheduleRenderJob (new SetPlaying (m_pipeline, m_videoSender, GST_STATE_NULL),
          QQuickWindow::BeforeSynchronizingStage);
    m_videoWidget->repaint();
}

QQuickWidget* RPIVideoDisplay::widget() const{
    return m_videoWidget;
}

int RPIVideoDisplay::videoId() const{
    return m_videoSender->videoId();
}
