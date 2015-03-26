function randomColor () {
	return '#' + (function co(lor){   return (lor +=
  [0,1,2,3,4,5,6,7,8,9,'a','b','c','d','e','f'][Math.floor(Math.random()*16)])
  && (lor.length == 6) ?  lor : co(lor); })('')
}

function clone(obj) {
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