open Jest;
open Expect;

open Belt.Result;

module DT = DomTestingLibrary;
module RT = ReactTestingLibrary;

let errorHandler = err => Js.log(err);

[@bs.get] external innerText: Dom.element => string = "";
[@bs.get] external innerHTML: Dom.element => string = "";

describe("Note Functionality Test", () => {
  test("Notes component renders successfully", () =>
    ReactTestingLibrary.render(<Notes initialNotes=["First note"] />)
    |> expect
    |> toMatchSnapshot
  );
  testAsync("When user adds a note, a new row appears", finish => {
    let tree =
      ReactTestingLibrary.render(<Notes initialNotes=["First note"] />);

    let container = tree |> ReactTestingLibrary.container;

    let waitForNote = () => tree |> ReactTestingLibrary.getByTestId("note-1");

    let waitForInputChange = () =>
      tree |> ReactTestingLibrary.getByValue("New Note");

    let options =
      DomTestingLibrary.WaitForElement.makeOptions(
        ~container,
        ~timeout=2000,
        (),
      );

    let event =
      EventRe.makeWithOptions("change", {
                                          "target": {
                                            "value": "New Note",
                                          },
                                        });

    (tree |> ReactTestingLibrary.getByPlaceholderText("Add new note here"))
    ->DomTestingLibrary.FireEvent.change(event);

    DomTestingLibrary.waitForElement(
      ~options,
      ~callback=waitForInputChange,
      (),
    )
    ->FutureJs.fromPromise(errorHandler)
    ->Future.flatMapOk(_ => {
        tree
        |> ReactTestingLibrary.getByTestId("add-note-button")
        |> DomTestingLibrary.FireEvent.click;

        DT.waitForElement(~callback=waitForNote, ~options, ())
        ->FutureJs.fromPromise(errorHandler);
      })
    ->Future.get(
        fun
        | Ok(node) =>
          node |> innerHTML |> expect |> toEqual("New Note") |> finish
        | Error(err) => {
            Js.log(err);
            fail("Note not found") |> finish;
          },
      );
  });
});