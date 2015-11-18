(function(undefined) {
  "use strict";
  // Doing somthing for renders
  console.log('Loading gvis.render')

  gvis.renders = function(render_container, render_type) {
    this.render_container = render_container;

    if (!render_type) {
      render_type = 'svg';
    }

    this.render_type = render_type;

    switch (this.render_type) {
      case 'canvas':
        this.renderer = 'render ' + this.render_container + ' by using canvas.';
      break;
      case 'map':
        this.renderer = 'render ' + this.render_container + ' by using map.';
      break;
      case 'svg':
      default:
        this.renderer = 'render ' + this.render_container + ' by using svg.';
      break;
    }
  }

  gvis.renders.prototype.render = function() {
    console.log(this.renderer);
  }
  
}).call(this)