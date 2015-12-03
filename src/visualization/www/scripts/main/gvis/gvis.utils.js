(function(undefined) {
  "use strict";
  console.log('Loading gvis.utils')

  // Initialize 
  gvis.utils = gvis.utils || {}


  // Generate random color string.
  gvis.utils.randomColor = function () {
    return '#' + (function co(lor){   return (lor +=
    [0,1,2,3,4,5,6,7,8,9,'a','b','c','d','e','f'][Math.floor(Math.random()*16)])
    && (lor.length == 6) ?  lor : co(lor); })('')
  }

  // Deep copy object and array in javascript (same as jQuery.extend(true, {}, obj))
  gvis.utils.clone = function (obj) {
    if (obj === null || typeof obj !== 'object') {
        return obj;
    }
 
    var temp = obj.constructor(); // give temp the original obj's constructor
    for (var key in obj) {
        temp[key] = this.clone(obj[key]);
    }
 
    return temp;
  }


  /*
  Example :
  template = 'translate({{x}}, {{y}})';

  return applyTemplate(template, {x:10, y:10})

  */
  gvis.utils.applyTemplate = function (template, objects) {
    for (var key in objects) {
      value = objects[key]
      template = template.replace(new RegExp("{{"+key+"}}", "g"), value)
    }
    
    return template;
  }

  /**
   * This function takes any number of objects as arguments, copies from each
   * of these objects each pair key/value into a new object, and finally
   * returns this object.
   *
   * The arguments are parsed from the last one to the first one, such that
   * when several objects have keys in common, the "earliest" object wins.
   *
   * Example:
   * ********
   *  > var o1 = {
   *  >       a: 1,
   *  >       b: 2,
   *  >       c: '3'
   *  >     },
   *  >     o2 = {
   *  >       c: '4',
   *  >       d: [ 5 ]
   *  >     };
   *  > gvis.utils.extend(o1, o2);
   *  > // Returns: {
   *  > //   a: 1,
   *  > //   b: 2,
   *  > //   c: '3',
   *  > //   d: [ 5 ]
   *  > // };
   *
   * @param  {object+} Any number of objects.
   * @return {object}  The merged object.
  */
  gvis.utils.extend = function() {
    var i,
        k,
        res = {},
        l = arguments.length;

    for (i = l - 1; i >= 0; i--)
      for (k in arguments[i])
        res[k] = arguments[i][k];

    return res;
  };


  /*
  Example :

  <g id='test'>
    <g>
    <rect width="50" height="50" x = '25' y = '25' fill='red'></rect> 
    <circle r=25 cx= 10 cy = 50 fill='blue'></circle>  
    <g> 
  </g>

  ****


  d3.select('#test')
  .attr('temp', function() {
    //moveGroup.apply(this, [100, 100])
    moveGroup.call(this, 100, 100)
    return 'temp'
  })

  */

  gvis.utils.moveGroup = function (cx, cy) {
      var box = this.getBBox();
      var x = box.x;  
      var y = box.y;
      var width = box.width;
      var height = box.height;

      d3.select(this).select('g')
      .attr('transform', translate(-x, -y))

      d3.select(this)
      .attr('transform', translate(cx-width/2, cy-height/2))
  }


  gvis.utils.make_listed_hash = function () {
      var list = [];
      var keys = [];
      var hash = {};
      return {
          push: function(k,v) {
              if (!hash[k]) {
                  keys.push(k);
                  list.push(v);
              }
              else {
                  var index = keys.indexOf(k);
                  list[index] = v;
              }
              hash[k] = v;
          },
          insert: function(pos,k,v) {
              if (!hash[k]) {
                  keys.splice(pos,0,k);
                  list.splice(pos,0,v);
                  hash[k] = v;
              }
          },
          remove: function(k) {
              if (!hash[k]) {
              }
              else {
                  delete hash[k];
                  var index = keys.indexOf(k);
                  keys.splice(index, 1);
                  list.splice(index, 1);
              }
          },
          update: function(k, v) {
              if (!hash[k]) {
              }
              else {
                  var index = keys.indexOf(k);
                  list[index] = v;
                  hash[k] = v;
              }
          },
          list: list,
          keys: keys,
          hash: hash
      };
  };



// /*
// Example :
// result = 'translate({{x}}, {{y}})';

// return template(result, {x:10}, {y:10})

// */
// function template(template) {
//   for (var i in arguments) {
//     if (i == 0) {
//       continue;
//     }
//     else {
//       d = arguments[i]
//       key = Object.keys(d)[0];
//       value = d[key]
//       template = template.replace(new RegExp("{{"+key+"}}", "g"), value)
//     }
//   }
  
//   return template;
// }

// /*
// Example :
// template = 'translate({{x}}, {{y}})';

// return applyTemplate(template, {x:10, y:10})

// */
gvis.utils.applyTemplate = function (template, objects) {
  for (var key in objects) {
    var value = objects[key]
    template = template.replace(new RegExp("{{"+key+"}}", "g"), value)
  }
  
  return template;
}


gvis.utils.icons = function(name) {
  var icons = {};

  if (!name) name = 'default';

  // adding icon 'default'
  icons['default'] = {svg:'', width:612, height:612}
  icons['default'].svg = '<path d="M230.724,181.208c-2.393,2.587-3.95,4.256-5.119,5.508C227.775,184.379,230.724,181.208,230.724,181.208z"/>\
    <path d="M336.962,200.875c7.956,9.792,11.906,21.337,11.906,34.634c0,9.514-2.727,18.666-8.151,27.512\
      c-2.977,5.007-6.898,9.848-11.795,14.465l-16.301,16.107c-15.634,15.356-25.732,28.958-30.35,40.865\
      c-4.618,11.878-6.927,27.54-6.927,46.957h36.275c0-17.108,1.947-30.044,5.814-38.807c3.866-8.763,12.323-19.444,25.37-32.102\
      c17.942-17.387,29.849-30.572,35.746-39.53s8.874-20.641,8.874-35.051c0-23.756-8.039-43.285-24.146-58.585\
      c-16.106-15.3-37.526-22.922-64.288-22.922c-28.931,0-51.686,8.929-68.266,26.789s-24.87,41.449-24.87,70.797h36.275\
      c0.667-17.665,3.478-31.184,8.346-40.559c8.679-16.83,24.369-25.259,47.068-25.259\
      C315.875,186.187,329.033,191.083,336.962,200.875z"/>\
    <path d="M612,306C612,137.004,474.995,0,306,0C137.004,0,0,137.004,0,306c0,168.995,137.004,306,306,306\
      C474.995,612,612,474.995,612,306z M27.818,306C27.818,152.36,152.36,27.818,306,27.818S584.182,152.36,584.182,306\
      S459.64,584.182,306,584.182S27.818,459.64,27.818,306z"/>\
    <rect x="274.51" y="415.214" width="40.559" height="42.367"/>'

  // adding icon 'user'
  icons['user'] = {svg:'', width:36, height:48}
  icons['user'].svg = '<path d="M32.102,19.679c1.211-0.259,2.328-1.26,2.51-3.48c0.15-1.817-0.316-2.753-1.004-3.23c1.91-7.785-3.358-9.309-3.358-9.309\
      s-3.937-6.679-11.618-1.992c-0.977,0.596-2.496,1.738-3.401,2.917c-1.453,1.546-2.442,4.059-2.597,8.129\
      c-0.938,0.358-1.675,1.264-1.492,3.484c0.192,2.321,1.403,3.309,2.677,3.511c1.72,4.189,5.116,7.641,9.135,7.641\
      C26.982,27.35,30.385,23.882,32.102,19.679z M22.954,25.397c-4.647,0-8.389-6.31-8.389-11.535c0-0.716,0.023-1.38,0.067-1.996\
      c6.797,0.297,10.817-2.428,13.005-4.792c2.075,2.068,3.153,4.68,3.704,6.619c0,0.056,0.001,0.111,0.001,0.168\
      C31.343,19.087,27.602,25.397,22.954,25.397z"/>\
    <path d="M20.292,33.297l0.538-2.227c0.062-0.257,0.195-0.477,0.375-0.646c-0.055-0.039-0.109-0.08-0.162-0.123l-4.806-3.979\
      l-2.925,0.684c-4.953,1.165-8.443,5.348-8.443,10.146v6.3c0,1.24,1.103,2.229,2.423,2.229h12.381l1.888-10.762\
      C20.713,34.908,20.092,34.125,20.292,33.297z"/>\
    <path d="M32.42,27.006l-2.885-0.684l-4.801,3.979c-0.053,0.043-0.105,0.084-0.16,0.123c0.181,0.169,0.314,0.389,0.377,0.646\
      l0.537,2.227c0.201,0.827-0.42,1.611-1.269,1.623l1.888,10.762h12.381c1.32,0,2.324-0.988,2.324-2.229v-6.3\
      C40.814,32.354,37.373,28.171,32.42,27.006z"/>';

  // adding icon 'movie'
  icons['movie'] = {svg:'', width:470, height:470}
  icons['movie'].svg = '<path d="M450.066,0v19.91h-44.5V0h-341.5v19.91h-44.5V0H0v469.633h19.566v-21.26h44.5v21.26h341.5v-21.26h44.5v21.26h19.566V0\
    H450.066z M64.066,396.635h-44.5v-42.443h44.5V396.635z M64.066,302.454h-44.5v-42.443h44.5V302.454z M64.066,208.273h-44.5V165.83\
    h44.5V208.273z M64.066,114.091h-44.5V71.648h44.5V114.091z M365.568,54.063v149.819H104.063V54.063H365.568z M104.063,415.568\
    V265.75h261.505v149.818H104.063z M450.066,396.635h-44.5v-42.443h44.5V396.635z M450.066,302.454h-44.5v-42.443h44.5V302.454z\
     M450.066,208.273h-44.5V165.83h44.5V208.273z M450.066,114.091h-44.5V71.648h44.5V114.091z"/>'

  if (!icons[name]) {
    return icons['default'];
  }
  else {
    return icons[name];
  }
}

gvis.utils.rotate = function(cx, cy, x, y, angle) {
  //X' = X cosB - Y sinB
  //Y' = X sinB + Y cosB
  var radians = (Math.PI / 180) * angle,
  cos = Math.cos(radians),
  sin = Math.sin(radians),
  nx = (cos * (x - cx)) - (sin * (y - cy)) + cx,
  ny = (sin * (x - cx)) + (cos * (y - cy)) + cy;
  return [nx, ny];
}


}).call(this)






