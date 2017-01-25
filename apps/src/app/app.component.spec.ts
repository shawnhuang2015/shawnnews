/* tslint:disable:no-unused-variable */

import { TestBed, async } from '@angular/core/testing';
import { AppModule } from './app.module';
import { AppComponent } from './app.component';
// import { HeaderComponent } from './header/header.component';
// import { StoriesComponent } from './stories/stories.component';
// import { FootComponent } from './foot/foot.component';

describe('AppComponent', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      declarations: [
        // AppComponent,
        // HeaderComponent,
        // StoriesComponent,
        // FootComponent
      ],
      imports: [
        AppModule
      ]
    });
    TestBed.compileComponents();
  });

  it('should create the app', async(() => {
    let fixture = TestBed.createComponent(AppComponent);
    // let app = fixture.componentInstance;
    let app = fixture.debugElement.componentInstance;
    expect(app).toBeTruthy();
  }));

  it(`should have as title 'app works!'`, async(() => {
    let fixture = TestBed.createComponent(AppComponent);
    let app = fixture.debugElement.componentInstance;
    expect(app.title).toEqual('Hello World app works!');
  }));

  it('should render title in a h1 tag', async(() => {
    let fixture = TestBed.createComponent(AppComponent);
    fixture.detectChanges();
    let compiled = fixture.debugElement.nativeElement;
    let app = fixture.componentInstance;

    expect(app).toBeTruthy();
    app.OnInit();
    expect(app.title).toContain('Test');
    expect(compiled.querySelector('h1').textContent).toContain('Xiaoke');
  }));
});
