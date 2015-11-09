(function(undefined) {
  "use strict";

  console.log('Loading gvis.settings')

  var settings = {
    skipType : ["reason", "level", "app", "phone_no", "location",
                  "friend", "complaint", "user", "device",
                  "enterprise", "warning", "bypass_complaint", "srtype",
                  "solution"]// not show type for specific types

    ,skipId : ["enterprise", "warning", "reason", "app", 
                "friend", "bypass_complaint", "srtype", "solution",
                "location"] // not show id for specific types

    ,skipAttr : {reason:{"*":true},
                    srtype:{category:true},
                    warning:{is_warning:true},
                    solution:{product_list:true},
                    phone_no:{has_friend:true,fraction_of_callees_has_friends:true}
                  }
  }

  gvis.settings = gvis.utils.extend(gvis.settings || {}, settings);
}).call(this)