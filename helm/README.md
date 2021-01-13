## HPCC Systems HELM Chart
- brief write up needed here
### Dependancies
- HPCC Systems is designed to deploy a local Elastic Stack instance for log processing. This dependency requires a local copy of the Elastic Stack Helm charts for ElasticSearch, Filebeats and optionally Kibana. Helm provides a convenient command to pull appropriate dependancies locally:
> helm dependency update

- The managed Elastic Stack instance can be  disabled by setting the following values to false:
>--set global.logging.managedElasticStack.enabled=false
and
>--set global.logging.managedElasticStack.kibanaenabled=false
