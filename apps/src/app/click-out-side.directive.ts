import { Directive, ElementRef, Output, EventEmitter, HostListener } from '@angular/core';

@Directive({
  selector: '[appClickOutSide]'
})
export class ClickOutSideDirective {

  @Output()
  public clickOutside = new EventEmitter();

  @HostListener('document:click', ['$event.target'])
  public onClick(targetElement) {
    const clickedInside = this._elementRef.nativeElement.contains(targetElement);
    if (!clickedInside) {
        this.clickOutside.emit(null);
        console.log('Click outside');
    } else {
      this.clickOutside.emit('clickoutside emit message');
      console.log('Click inside');
    }
  }


  constructor(private _elementRef: ElementRef) { }


}
