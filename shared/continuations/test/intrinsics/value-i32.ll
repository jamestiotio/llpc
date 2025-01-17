; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 2
; RUN: opt --opaque-pointers=0 --verify-each -passes='add-types-metadata,dxil-cont-post-process,lint' -S %s 2>%t.stderr | FileCheck %s
; RUN: count 0 < %t.stderr

declare i32 @_AmdValueI32Count(%struct.Payload*)
declare i32 @_AmdValueGetI32(%struct.Payload*, i32)
declare void @_AmdValueSetI32(%struct.Payload*, i32, i32)

%struct.Payload = type { float, i32, i64, i32 }

define i32 @count(%struct.Payload* %pl) {
; CHECK-LABEL: define i32 @count
; CHECK-SAME: (%struct.Payload* [[PL:%.*]]) !types !0 {
; CHECK-NEXT:    ret i32 5
;
  %val = call i32 @_AmdValueI32Count(%struct.Payload* %pl)
  ret i32 %val
}

define i32 @get(%struct.Payload* %pl) {
; CHECK-LABEL: define i32 @get
; CHECK-SAME: (%struct.Payload* [[PL:%.*]]) !types !0 {
; CHECK-NEXT:    [[TMP1:%.*]] = bitcast %struct.Payload* [[PL]] to i32*
; CHECK-NEXT:    [[TMP2:%.*]] = getelementptr i32, i32* [[TMP1]], i32 2
; CHECK-NEXT:    [[TMP3:%.*]] = load i32, i32* [[TMP2]], align 4
; CHECK-NEXT:    ret i32 [[TMP3]]
;
  %val = call i32 @_AmdValueGetI32(%struct.Payload* %pl, i32 2)
  ret i32 %val
}

define void @set(%struct.Payload* %pl, i32 %val) {
; CHECK-LABEL: define void @set
; CHECK-SAME: (%struct.Payload* [[PL:%.*]], i32 [[VAL:%.*]]) !types ![[SETTYPE:[0-9]+]] {
; CHECK-NEXT:    [[TMP1:%.*]] = bitcast %struct.Payload* [[PL]] to i32*
; CHECK-NEXT:    [[TMP2:%.*]] = getelementptr i32, i32* [[TMP1]], i32 2
; CHECK-NEXT:    store i32 [[VAL]], i32* [[TMP2]], align 4
; CHECK-NEXT:    ret void
;
  call void @_AmdValueSetI32(%struct.Payload* %pl, i32 2, i32 %val)
  ret void
}
