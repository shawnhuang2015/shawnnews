require([], function(){
  var data = {
    "nodes": [
      {"id": "0", "type": "usr", "attr": {"a1": 1, "a2": 2}, "style": {"size": 100, "shape": "square", "fill": "#ff5500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "1", "type": "tag", "attr": {"a1": 1, "a2": 2}, "style": {"size": 100, "shape": "circle", "fill": "#005500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.8}},
      {"id": "2", "type": "usr", "attr": {"a1": 1, "a2": 2}, "style": {"size": 10, "shape": "cross", "fill": "#0055ff", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.7}},
      {"id": "3", "type": "movie", "attr": {"a1": 1, "a2": 2}, "style": {"size": 60, "shape": "circle", "fill": "#ff5500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.6}},
      {"id": "4", "type": "tag", "attr": {"a1": 1, "a2": 2}, "style": {"size": 10, "shape": "diamond", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "5", "type": "movie", "attr": {"a1": 1, "a2": 2}, "style": {"size": 60, "shape": "circle", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}},
      {"id": "6", "type": "usr", "attr": {"a1": 1, "a2": 2}, "style": {"size": 10,"shape": "cross", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.5}},
      {"id": "7", "type": "tag", "attr": {"a1": 1, "a2": 2}, "style": {"size": 20, "shape": "circle", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.4}},
      {"id": "8", "type": "usr", "attr": {"a1": 1, "a2": 2, "a3": 4, "a4": 5}, "style": {"size": 60, "shape": "square", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "9", "type": "tag", "attr": {"a1": 1, "a2": 2}, "style": {"size": 10, "shape": "diamond", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.9}},
      {"id": "10", "type": "usr", "attr": {"a1": 1, "a2": 2}, "style": {"size": 60, "shape": "square", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.8}},
      {"id": "11", "type": "movie", "attr": {"a1": 1, "a2": 2}, "style": {"size": 10, "shape": "diamond", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.7}},
      {"id": "12", "type": "usr", "attr": {"a1": 1, "a2": 2}, "style": {"size": 60, "shape": "cross", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.6}}],
    "links": [
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "1", "type": "tag"}, "type": "abc", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "2", "type": "usr"}, "type": "adf", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": false},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "3", "type": "movie"}, "type": "dds", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": true},
      {"source": {"id": "4", "type": "tag"}, "target": {"id": "0", "type": "usr"}, "type": "adx", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "0", "type": "usr"}, "type": "fdd", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": false},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "6", "type": "usr"}, "type": "eew", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.5}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "3", "type": "movie"}, "type": "eer", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.4}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "4", "type": "tag"}, "type": "dff", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.3}, "directed": false},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "1", "type": "tag"}, "type": "sdf", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "6", "type": "usr"}, "type": "sdd", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "4", "type": "tag"}, "type": "eeq", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": false},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "5", "type": "movie"}, "type": "cca", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "6", "type": "usr"}, "type": "dfa", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": true},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "5", "type": "movie"}, "type": "ccq", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.5}, "directed": false},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "6", "type": "usr"}, "type": "ccb", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": true, "opacity": 0.4}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "6", "type": "usr"}, "type": "ccr", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.3}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "7", "type": "tag"}, "type": "ccw", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": false},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "8", "type": "usr"}, "type": "ssa", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "9", "type": "tag"}, "type": "ssb", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": true},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "10", "type": "usr"}, "type": "bbc", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": false},
      {"source": {"id": "4", "type": "tag"}, "target": {"id": "11", "type": "movie"}, "type": "bbs", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "12", "type": "usr"}, "type": "bbc", "attr": {"aa1": 3, "aa2": 4}, "style": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.5}, "directed": true}]
  };

  this.test = new gvis(config);

  test.read(data);

  console.log(test.scope.graph.nodes());
  // for (var node in data.nodes) {
  //   test.addNode(data.nodes[node]);
  // }

  // for (var link in data.links) {
  //   test.addLink(data.links[link]);
  // }


  window.addEventListener('resize', function() {
    console.log('resizing')
  });
});
