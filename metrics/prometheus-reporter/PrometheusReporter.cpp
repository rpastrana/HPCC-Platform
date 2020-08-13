/*
 * PrometheusReporter.cpp
 *
 *  Created on: Aug 13, 2020
 *      Author: ubuntu
 */

#include "PrometheusReporter.h"
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

PrometheusReporter::PrometheusReporter() {
	// TODO Auto-generated constructor stub

}

PrometheusReporter::~PrometheusReporter() {
	// TODO Auto-generated destructor stub
}

void usage(const char *progname)
{
}

int main(int argc, char **argv)
{
	using namespace prometheus;

	  // create an http server running on port 8080
	  Exposer exposer{"127.0.0.1:8787"};

	  // create a metrics registry with component=main labels applied to all its
	  // metrics
	  auto registry = std::make_shared<Registry>();

	  // add a new counter family to the registry (families combine values with the
	  // same name, but distinct label dimensions)
	  auto& counter_family = BuildCounter()
	                             .Name("time_running_seconds_total")
	                             .Help("How many seconds is this server running?")
	                             .Labels({{"label", "value"}})
	                             .Register(*registry);

	  // add a counter to the metric family
	  auto& second_counter = counter_family.Add(
	      {{"another_label", "value"}, {"yet_another_label", "value"}});

	  // ask the exposer to scrape the registry on incoming scrapes
	  exposer.RegisterCollectable(registry);

	  for (;;)
	  {
	    std::this_thread::sleep_for(std::chrono::seconds(1));
	    // increment the counter by one (second)
	    second_counter.Increment();
	  }
	  return 0;
}
