// Copyright © 2026 Peter Cerno. All rights reserved.

#include "util/fxmacrodata.h"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace trader {
namespace {

std::string TrimTrailingSlash(std::string value) {
  while (!value.empty() && value.back() == '/') {
    value.pop_back();
  }
  return value;
}

std::string EnsureLeadingSlash(const std::string& path) {
  if (!path.empty() && path.front() == '/') {
    return path;
  }
  return "/" + path;
}

std::string Lower(std::string value) {
  for (char& ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

std::string Encode(const std::string& value) {
  std::ostringstream out;
  out << std::uppercase << std::hex;
  for (unsigned char ch : value) {
    if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
      out << static_cast<char>(ch);
    } else {
      out << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
    }
  }
  return out.str();
}

}  // namespace

FxMacroDataClient::FxMacroDataClient(std::string api_key,
                                     std::string base_url,
                                     Transport transport)
    : api_key_(std::move(api_key)),
      base_url_(TrimTrailingSlash(std::move(base_url))),
      transport_(std::move(transport)) {}

std::string FxMacroDataClient::BuildUrl(const std::string& path,
                                        QueryParams params) const {
  if (!api_key_.empty()) {
    params.push_back({"api_key", api_key_});
  }

  std::ostringstream url;
  url << base_url_ << EnsureLeadingSlash(path);
  char separator = '?';
  for (const auto& param : params) {
    url << separator << Encode(param.first) << '=' << Encode(param.second);
    separator = '&';
  }
  return url.str();
}

std::string FxMacroDataClient::Send(const std::string& method,
                                    const std::string& path,
                                    QueryParams params,
                                    const std::string& body) const {
  if (!transport_) {
    throw std::runtime_error("FxMacroDataClient requires a transport callback");
  }
  return transport_(method, BuildUrl(path, std::move(params)), body);
}

std::string FxMacroDataClient::DataCatalogue(const std::string& currency,
                                             QueryParams params) const {
  return Send("GET", "/data_catalogue/" + Encode(Lower(currency)),
              std::move(params));
}

std::string FxMacroDataClient::Announcements(const std::string& currency,
                                             const std::string& indicator,
                                             QueryParams params) const {
  return Send("GET", "/announcements/" + Encode(Lower(currency)) + "/" +
                        Encode(indicator),
              std::move(params));
}

std::string FxMacroDataClient::LatestAnnouncements(const std::string& currency,
                                                   QueryParams params) const {
  return Send("GET", "/announcements/" + Encode(Lower(currency)) + "/latest",
              std::move(params));
}

std::string FxMacroDataClient::AnnouncementChanges(QueryParams params) const {
  return Send("GET", "/announcements/changes", std::move(params));
}

std::string FxMacroDataClient::Calendar(const std::string& currency,
                                        QueryParams params) const {
  return Send("GET", "/calendar/" + Encode(Lower(currency)), std::move(params));
}

std::string FxMacroDataClient::Predictions(const std::string& currency,
                                           const std::string& indicator,
                                           QueryParams params) const {
  return Send("GET", "/predictions/" + Encode(Lower(currency)) + "/" +
                        Encode(indicator),
              std::move(params));
}

std::string FxMacroDataClient::Forex(const std::string& base,
                                     const std::string& quote,
                                     QueryParams params) const {
  return Send("GET", "/forex/" + Encode(Lower(base)) + "/" +
                        Encode(Lower(quote)),
              std::move(params));
}

std::string FxMacroDataClient::Cot(const std::string& currency,
                                   QueryParams params) const {
  return Send("GET", "/cot/" + Encode(Lower(currency)), std::move(params));
}

std::string FxMacroDataClient::Commodity(const std::string& indicator,
                                         QueryParams params) const {
  return Send("GET", "/commodities/" + Encode(indicator), std::move(params));
}

std::string FxMacroDataClient::CommoditiesLatest(QueryParams params) const {
  return Send("GET", "/commodities/latest", std::move(params));
}

std::string FxMacroDataClient::Curves(const std::string& currency,
                                      QueryParams params) const {
  return Send("GET", "/curves/" + Encode(Lower(currency)), std::move(params));
}

std::string FxMacroDataClient::CurveProxies(const std::string& currency,
                                            QueryParams params) const {
  return Send("GET", "/curve_proxies/" + Encode(Lower(currency)),
              std::move(params));
}

std::string FxMacroDataClient::ForwardCurves(const std::string& currency,
                                             QueryParams params) const {
  return Send("GET", "/forward_curves/" + Encode(Lower(currency)),
              std::move(params));
}

std::string FxMacroDataClient::RateDifferentials(const std::string& base,
                                                 const std::string& quote,
                                                 QueryParams params) const {
  return Send("GET", "/rate_differentials/" + Encode(Lower(base)) + "/" +
                        Encode(Lower(quote)),
              std::move(params));
}

std::string FxMacroDataClient::ForwardDifferentials(const std::string& base,
                                                    const std::string& quote,
                                                    QueryParams params) const {
  return Send("GET", "/forward_differentials/" + Encode(Lower(base)) + "/" +
                        Encode(Lower(quote)),
              std::move(params));
}

std::string FxMacroDataClient::MarketSessions(QueryParams params) const {
  return Send("GET", "/market_sessions", std::move(params));
}

std::string FxMacroDataClient::RiskSentiment(QueryParams params) const {
  return Send("GET", "/risk_sentiment", std::move(params));
}

std::string FxMacroDataClient::News(const std::string& currency,
                                    QueryParams params) const {
  return Send("GET", "/news/" + Encode(Lower(currency)), std::move(params));
}

std::string FxMacroDataClient::PressReleases(const std::string& currency,
                                             QueryParams params) const {
  return Send("GET", "/press-releases/" + Encode(Lower(currency)),
              std::move(params));
}

std::string FxMacroDataClient::Graphql(const std::string& query,
                                       const std::string& variables_json) const {
  return Send("POST", "/graphql", {},
              "{\"query\":\"" + query + "\",\"variables\":" + variables_json + "}");
}

std::string FxMacroDataClient::Request(const std::string& path,
                                       QueryParams params,
                                       std::string method,
                                       std::string body) const {
  return Send(std::move(method), path, std::move(params), body);
}

}  // namespace trader
