#include "nvmrdisplay.h"
#include "ui_nvmrdisplay.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QRunnable>

#include <log4cxx/logger.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "org.nvmr.nvmrdisplay" );

class SetPlaying : public QRunnable
{
public:
  SetPlaying(GstElement *);
  ~SetPlaying();

  void run ();

private:
  GstElement * pipeline_;
};

SetPlaying::SetPlaying (GstElement * pipeline)
{
  this->pipeline_ = pipeline ? static_cast<GstElement *> (gst_object_ref (pipeline)) : NULL;
}

SetPlaying::~SetPlaying ()
{
  if (this->pipeline_)
    gst_object_unref (this->pipeline_);
}

void
SetPlaying::run ()
{
  if (this->pipeline_)
    gst_element_set_state (this->pipeline_, GST_STATE_PLAYING);
}

NVMRDisplay::NVMRDisplay(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NVMRDisplay),
    m_numberEntered( 0 ),
    m_currentCommand( CurrentCommand::NoCommand )
{
    ui->setupUi(this);

    QTimer::singleShot( 1000, this, &NVMRDisplay::doOpenglStuff );
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
  //g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
//  LOG4CXX_DEBUG( logger, "bus message: " << GST_MESSAGE_TYPE_NAME (message) );

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

void NVMRDisplay::doOpenglStuff(){

    ui->quickWidget->setSource(QUrl("qrc:/quickwidget/video.qml"));

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
      }



      QQuickItem* quickRoot = ui->quickWidget->rootObject();
      g_object_set( sinkQml,
                    "widget", quickRoot->findChild<QQuickItem*>( "videoItem" ),
                    nullptr );
      for( QObject* obj : quickRoot->children() ){
          LOG4CXX_DEBUG( logger, "object name is: " << obj->objectName().toStdString() );
      }

//      gst_bin_add_many (GST_BIN (pipeline), src, convert, sink, NULL);
//      gst_element_link_many (src, convert, sink);


     ui->quickWidget->quickWindow()->scheduleRenderJob (new SetPlaying (pipeline),
          QQuickWindow::BeforeSynchronizingStage);


//      GstStateChangeReturn sret = gst_element_set_state (pipeline,
//          GST_STATE_PLAYING);
//      if (sret == GST_STATE_CHANGE_FAILURE) {
//        gst_element_set_state (pipeline, GST_STATE_NULL);
//        gst_object_unref (pipeline);
//        LOG4CXX_ERROR( logger, "Unable to play pipeline" );
//        // Exit application
//        //QTimer::singleShot(0, QApplication::activeWindow(), SLOT(quit()));
//      }
}
