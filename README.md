`oanda-to-tickq`
===============

`oanda-to-tickq` is part of the GreenFX trading infrastructure, and is
distributed under the terms of the GNU General Public License (v3 or
later).

`oanda-to-tickq` reads from the OANDA streaming service (via REST
APIs), and publishes foreign exchange tick information
(timestamp/bid/ask for a given currency pair) as a J-SON object to an
AM-Q message bus we will call `tickq`.

`oanda-to-tickq` requires three envionment variables to be set in order
to read from OANDA: `OANDA_STREAM_DOMAIN`, `OANDA_ACCESS_TOKEN`, and
`OANDA_ACCOUNT_ID`.  See the OANDA streaming API developer
documentation for details on how to set these.

`oanda-to-tickq` also requires two environment variables to be set in
order to talk to `tickq`: `AMQ_USER` and `AMQ_PASSWORD`.

As this program is intended to run as a microservice within an
OpenShift/kubernetes hosted infrastructure, the hostname of the AM-Q
broker is hard coded as `broker-amq-tcp`.

The currency pairs read by the `oanda-to-tickq` are currently
hardcoded in `main.cc`.

Anthony Green - anthony@atgreen.org




