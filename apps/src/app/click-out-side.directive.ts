import { Directive, ElementRef, Output, EventEmitter, HostListener, HostBinding } from '@angular/core';

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


  constructor(private _elementRef: ElementRef) {
    this.valueValue = 'Host binding value';
    this.colorValue = '#f00';
    this.testClass = true;
  }


}
