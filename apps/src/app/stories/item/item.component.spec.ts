/* tslint:disable:no-unused-variable */
// import { Component, CUSTOM_ELEMENTS_SCHEMA } from '@angular/core';
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';

import { ItemComponent } from './item.component';
import { AppModule } from '../../app.module';

describe('ItemComponent', () => {
  let component: ItemComponent;
  let fixture: ComponentFixture<ItemComponent>;

  beforeEach(async(() => {
    TestBed
    .configureTestingModule({
      // declarations: [TestComponentWrapper],
      imports: [AppModule],
      // schemas: [CUSTOM_ELEMENTS_SCHEMA]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ItemComponent);
    component = fixture.componentInstance;

    component.item = {
      'id': 13445232, 'title': 'How Deutsche Bank Made a $462M Loss Disappear', 'points': 29, 'user': 'mrkibo',
      'time': 1484932627, 'time_ago': 'an hour ago', 'comments_count': 2,
      'type': 'link', 'url': 'https://www.bloomberg.com/news/features/2017-01-19/how-deutsche-bank-made-367-million-disappear', 'domain': 'bloomberg.com'
    };
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();

    let compiled = fixture.nativeElement;
    expect(compiled.querySelector('.title').textContent).toContain('How Deutsche Bank Made a $462M Loss Disappear');
    expect(compiled.querySelector('.domain').textContent).toContain('www.bloomberg.com');
  });

  // it('wrapper should create', () => {
  //   let wFixture = TestBed.createComponent(TestComponentWrapper);
  //   let wComponent = wFixture.debugElement.children[0].nativeElement;
  //   wFixture.detectChanges();

  //   // component = fixture.debugElement.children[0].componentInstance;

  //   expect(wComponent).toBeTruthy();
  //   console.log(wComponent);

  //   let compiled = wFixture.nativeElement;
  //   expect(compiled.textContent).toContain('How Deutsche Bank Made a $462M Loss Disappear');
  // });
});

// @Component({
//   selector: 'test-component-wrapper',
//   template: '<story-item [item]="item"></story-item>'
// })

// class TestComponentWrapper {
//   item = {
//     'id': 13445232, 'title': 'How Deutsche Bank Made a $462M Loss Disappear', 'points': 29, 'user': 'mrkibo',
//     'time': 1484932627, 'time_ago': 'an hour ago', 'comments_count': 2,
//     'type': 'link', 'url': 'https://www.bloomberg.com/news/features/2017-01-19/how-deutsche-bank-made-367-million-disappear', 'domain': 'bloomberg.com'
//   };
// }
