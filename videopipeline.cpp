#include "videopipeline.h"

VideoPipeline::VideoPipeline(QObject *parent) : QObject(parent)
{
    m_pipeline = gst_pipeline_new( "pipeline" );
}

VideoPipeline::~VideoPipeline(){
    gst_object_unref( GST_OBJECT( m_pipeline ) );
}

void VideoPipeline::play(){
    gst_element_set_state (m_pipeline, GST_STATE_PLAYING);
}

void VideoPipeline::stop(){
    gst_element_set_state (m_pipeline, GST_STATE_NULL);
}
