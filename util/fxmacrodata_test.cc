// Copyright © 2026 Peter Cerno. All rights reserved.

#include "util/fxmacrodata.h"

#include <string>

#include "gtest/gtest.h"

namespace trader {

TEST(FxMacroDataClientTest, BuildsAuthenticatedRequests) {
  std::string method;
  std::string url;
  std::string body;
  FxMacroDataClient client(
      "test-key", "https://api.fxmacrodata.com/v1/",
      [&](const std::string& m, const std::string& u, const std::string& b) {
        method = m;
        url = u;
        body = b;
        return std::string("{\"ok\":true}");
      });

  EXPECT_EQ(client.Calendar("USD", {{"days_ahead", "30"}}), "{\"ok\":true}");
  EXPECT_EQ(method, "GET");
  EXPECT_EQ(url,
            "https://api.fxmacrodata.com/v1/calendar/usd?days_ahead=30&api_key=test-key");

  client.RateDifferentials("EUR", "USD", {{"tenor", "2y"}});
  EXPECT_EQ(url,
            "https://api.fxmacrodata.com/v1/rate_differentials/eur/usd?tenor=2y&api_key=test-key");

  client.Graphql("query { marketSessions { name } }");
  EXPECT_EQ(method, "POST");
  EXPECT_EQ(url, "https://api.fxmacrodata.com/v1/graphql?api_key=test-key");
  EXPECT_NE(body.find("marketSessions"), std::string::npos);
}

}  // namespace trader
