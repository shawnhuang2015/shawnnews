import { Directive, ElementRef, Output, EventEmitter, HostListener, HostBinding, NgZone } from '@angular/core';

import { NewsApiService } from './news-api.service';

@Directive({
  selector: '[appClickOutSide]'
})
export class ClickOutSideDirective {

  @Output()
  public clickOutside = new EventEmitter();

  @HostBinding('attr.value') valueValue: string;
  @HostBinding('style.color') colorValue: string;
  @HostBinding('class.test') testClass: boolean;

  @HostListener('mousedown', ['$event'])
  onMousedown(e: any) {
    console.log('on mousedown in directive', e);
  }

  @HostListener('document:click', ['$event.target'])
  public onClick(targetElement) {
    const clickedInside = this._elementRef.nativeElement.contains(targetElement);
    if (!clickedInside) {
      this.clickOutside.emit(null);
    } else {
      this.clickOutside.emit('clickoutside emit message');
    }
  }

  // @HostListener('mousemove', ['$event.target', '$event'])
  // public onMouseMover(targetElement, event) {
  //   event.preventDefault();
  //   this.ngZone.runOutsideAngular(() => {
  //     console.log(targetElement);
  //   });
  // }

  constructor(private _elementRef: ElementRef, private service: NewsApiService, private ngZone: NgZone) {
    this.valueValue = 'Host binding value';
    this.colorValue = '#f00';
    this.testClass = true;

    console.log(this.service.testService(), this._elementRef);

    // window.document.addEventListener('mousemove', this.mouseMove.bind(this));
  }

  mouseMove(event) {
    console.log(event);
  }
}
