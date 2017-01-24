/* tslint:disable:no-unused-variable */
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';

import { StoriesComponent } from './stories.component';
import { AppModule } from '../app.module';
import { RouterTestingModule } from '@angular/router/testing';
import { ActivatedRoute } from '@angular/router';
import { BehaviorSubject } from 'rxjs/BehaviorSubject';

// import { ItemComponent } from './item/item.component';

describe('StoriesComponent', () => {
  let component: StoriesComponent;
  let fixture: ComponentFixture<StoriesComponent>;

  let params: BehaviorSubject<any>;
  let data: BehaviorSubject<any>;

  beforeEach(async(() => {
    params = new BehaviorSubject({
      page: 1
    });

    data = new BehaviorSubject({
      storiesType: 'newest'
    });

    TestBed.configureTestingModule({
      // declarations: [ StoriesComponent, ItemComponent ]
      imports: [
        AppModule,
        // RouterTestingModule.withRoutes([
        //   { path: 'home', component: StoriesComponent }
        // ])
      ],
      providers: [
        { provide: ActivatedRoute, useValue: { params, data } }
      ],
    })
      .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(StoriesComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    params.next({
      page: 2
    });
    expect(component).toBeTruthy();
  });

  it('should update', () => {
    data.next({
      storiesType: 'ask'
    });
    expect(component).toBeTruthy();
  });
});
