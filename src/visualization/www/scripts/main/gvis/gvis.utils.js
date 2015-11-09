// Generate random color string.
function randomColor () {
	return '#' + (function co(lor){   return (lor +=
  [0,1,2,3,4,5,6,7,8,9,'a','b','c','d','e','f'][Math.floor(Math.random()*16)])
  && (lor.length == 6) ?  lor : co(lor); })('')
}

//Binding a function to an object.
function bind(obj, fn){
  var slice = [].slice;

  if ('string' == typeof fn) fn = obj[fn];
  if ('function' != typeof fn) throw new Error('bind() requires a function');
  var args = [].slice.call(arguments, 2);
  return function(){
    return fn.apply(obj, args.concat(slice.call(arguments)));
  }
}

// Deep copy object and array in javascript (same as jQuery.extend(true, {}, obj))
function cloneOld(obj) {
	if(obj == null || typeof(obj) != 'object')
		return obj;

	var temp = obj.constructor();

	if (obj instanceof Array) {
		obj.forEach(function(n) {
			temp.push(clone(n));
		}) 
	}
	else {
		for(var key in obj) {
			if(obj.hasOwnProperty(key)) {
				temp[key] = clone(obj[key]);
			}
		}
	}
	return temp;

}

function clone(obj) {
    if (obj === null || typeof obj !== 'object') {
        return obj;
    }
 
    var temp = obj.constructor(); // give temp the original obj's constructor
    for (var key in obj) {
        temp[key] = clone(obj[key]);
    }
 
    return temp;
}

/*
Example :
result = 'translate({{x}}, {{y}})';

return template(result, {x:10}, {y:10})

*/
function template(template) {
  for (var i in arguments) {
    if (i == 0) {
      continue;
    }
    else {
      d = arguments[i]
      key = Object.keys(d)[0];
      value = d[key]
      template = template.replace(new RegExp("{{"+key+"}}", "g"), value)
    }
  }
  
  return template;
}

/*
Example :
template = 'translate({{x}}, {{y}})';

return applyTemplate(template, {x:10, y:10})

*/
function applyTemplate(template, objects) {
  for (var key in objects) {
    value = objects[key]
    template = template.replace(new RegExp("{{"+key+"}}", "g"), value)
  }
  
  return template;
}


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

function moveGroup(cx, cy) {
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


function make_listed_hash() {
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






