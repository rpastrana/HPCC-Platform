#ifndef _ESPWIZ_Metrics_HPP_
#define _ESPWIZ_Metrics_HPP_

#include "metrics_esp.ipp"

class CmetricsEx : public Cmetrics
{
private:
    Owned<IPropertyTree> m_serviceCfg;
public:
    IMPLEMENT_IINTERFACE

    CmetricsEx();
    virtual ~CmetricsEx();
    virtual void init(IPropertyTree *cfg, const char *process, const char *service);
    //virtual bool onPrometheus(IEspContext &context,  IEspPrometheusMetricsRequest &req, IEspPrometheusMetricsResponse &resp);
};


class CmetricsSoapBindingEx : public CmetricsSoapBinding
{
    //StringBuffer m_authType;

public:
	CmetricsSoapBindingEx(IPropertyTree *cfg, const char *name, const char *process, http_soap_log_level llevel=hsl_none) : CmetricsSoapBinding(cfg, name, process, llevel)
    {
        /*StringBuffer xpath;
        xpath.appendf("Software/EspProcess[@name='%s']/Authentication/@method", process);
        const char* method = cfg->queryProp(xpath);
        if (method && *method)
            m_authType.append(method);*/
    }

	virtual int onGet(CHttpRequest* request, CHttpResponse* response) override
	{
		Owned<IMultiException> me = MakeMultiException("metrics");

		try
		{
			IEspContext *context = request->queryContext();
			IProperties *parms = request->queryParameters();

			//context->ensureFeatureAccess(WSECL_ACCESS, SecAccess_Full, -1, WSECL_ACCESS_DENIED);

			const char *thepath = request->queryPath();

			StringBuffer serviceName;
			//firstPathNode(thepath, serviceName);

            if (strieq(thepath, "/metrics/"))
			{
            	unsigned randval = getRandom();
            	//StringBuffer path;
            	//request->getPath(path);
            	//request->setPath("/metrics/prometheus");
            	//request->setMethod("prometheus");
            	//request->parseEspPathInfo(true);
            	StringBuffer metrics;
            	metrics.setf("# HELP esp_myesp_current_requests Count of requests being processed now\n\
# TYPE esp_myesp_current_requests counter\n\
esp_myesp_current_requests %u \n", randval);
				response->setContent(metrics.str()
/*
# HELP exposer_scrapes_total Number of times metrics were scraped\n\
# TYPE exposer_scrapes_total counter\n\
exposer_scrapes_total 20.000000\n\
# HELP exposer_request_latencies Latencies of serving scrape requests, in microseconds\n\
# TYPE exposer_request_latencies summary\n \
exposer_request_latencies_count 20\n\
exposer_request_latencies_sum 6125.000000\n\
exposer_request_latencies{quantile=\"0.500000\"} 259.000000\n\
exposer_request_latencies{quantile=\"0.900000\"} 388.000000\n\
exposer_request_latencies{quantile=\"0.990000\"} 498.000000\n\
# HELP time_running_seconds_total How many seconds is this server running?\n\
# TYPE time_running_seconds_total counter\n\
time_running_seconds_total{label=\"value\",another_label=\"value\",yet_another_label=\"value\"} 19.000000\n"
*/
);

response->setContentType("text/plain");
response->setStatus("200 ok");
response->send();
return 0;
 //return EspHttpBinding::onGetService(*context, request, response, serviceName.str(), "prometheus", path.str());
            	/*
				StringBuffer url;
				url.append("/metrics/prometheus");
				response->redirect(*request, url);
				return 0;*/
			}
            else
                return EspHttpBinding::onGet(request, response);
		}
		catch (IMultiException* mex)
		{
			me->append(*mex);
			mex->Release();
		}
		catch (IException* e)
		{
			me->append(*e);
		}
		catch (...)
		{
			me->append(*MakeStringExceptionDirect(-1, "Unknown Exception"));
		}

		response->handleExceptions(getXslProcessor(), me, "metrics", "", StringBuffer(getCFD()).append("./smc_xslt/exceptions.xslt").str(), false);
		return 0;
	}
};

#endif // _ESPWIZ_Metrics_HPP_
