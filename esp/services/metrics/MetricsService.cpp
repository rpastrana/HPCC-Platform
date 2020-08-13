#include "MetricsService.hpp"

CmetricsEx::CmetricsEx()
{
}

CmetricsEx::~CmetricsEx()
{
}

void CmetricsEx::init(IPropertyTree *cfg, const char *process, const char *service)
{
    if(cfg == nullptr)
        throw MakeStringException(-1, "Cannot initialize CmetricsEx, cfg is NULL");

    StringBuffer xpath;
    xpath.appendf("Software/EspProcess[@name=\"%s\"]/EspService[@name=\"%s\"]", process, service);
    m_serviceCfg.setown(cfg->getPropTree(xpath.str()));

#ifdef _DEBUG
    StringBuffer thexml;
    toXML(m_serviceCfg, thexml,0,0);
    DBGLOG("^^^^^^%s", thexml.str());
#endif
}

/*bool CmetricsEx::onPrometheus(IEspContext &context,  IEspPrometheusMetricsRequest &req, IEspPrometheusMetricsResponse &resp)
{
	resp.setContent("# HELP exposer_transferred_bytes_total Transferred bytes to metrics services\n# TYPE exposer_transferred_bytes_total counter\nexposer_transferred_bytes_total 6567.000000\n# HELP exposer_scrapes_total Number of times metrics were scraped\n# TYPE exposer_scrapes_total counter\nexposer_scrapes_total 20.000000\n# HELP exposer_request_latencies Latencies of serving scrape requests, in microseconds\n# TYPE exposer_request_latencies summary\nexposer_request_latencies_count 20\nexposer_request_latencies_sum 6125.000000\nexposer_request_latencies{quantile=\"0.500000\"} 259.000000\n");
	exposer_request_latencies{quantile="0.900000"} 388.000000
	exposer_request_latencies{quantile="0.990000"} 498.000000
	# HELP time_running_seconds_total How many seconds is this server running?
	# TYPE time_running_seconds_total counter
	time_running_seconds_total{label="value",another_label="value",yet_another_label="value"} 19.000000");

	resp->setContentType("text/plain");
	resp->setStatus("200 ok");
	resp->send();
	return true;
}*/

