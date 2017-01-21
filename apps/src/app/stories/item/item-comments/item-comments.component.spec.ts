/* tslint:disable:no-unused-variable */
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';
import { Location } from '@angular/common';

import { ItemCommentsComponent } from './item-comments.component';
import { AppModule } from '../../../app.module';

import { RouterTestingModule } from '@angular/router/testing';
import { ActivatedRoute } from '@angular/router';
import { BehaviorSubject } from 'rxjs/BehaviorSubject';


describe('ItemCommentsComponent', () => {
  let component: ItemCommentsComponent;
  let fixture: ComponentFixture<ItemCommentsComponent>;

  let paramas = new BehaviorSubject({
    id : 1
  });
  let params = new BehaviorSubject({
    id : 1
  });

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      imports: [AppModule],
      providers: [
        {provide: ActivatedRoute, useValue: {paramas, params}}
      ]
      // declarations: [ ItemCommentsComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ItemCommentsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
