/// <reference path='fourslash.ts' />

// @Filename: b.ts
/////*alias1Definition*/import alias1 = require("fileb");
////module Module {
////    /*alias2Definition*/export import alias2 = alias1;
////}
////
////// Type position
////var t1: /*alias1Type*/alias1.IFoo;
////var t2: Module./*alias2Type*/alias2.IFoo;
////
////// Value posistion
////var v1 = new /*alias1Value*/alias1.Foo();
////var v2 = new Module./*alias2Value*/alias2.Foo();


// @Filename: a.ts
////export class Foo {
////    private f;
////}
////export interface IFoo {
////    x;
////}

verify.goToDefinition([
    [["alias1Type", "alias1Value"], "alias1Definition"],
    [["alias2Type", "alias2Value"], "alias2Definition"]
]);